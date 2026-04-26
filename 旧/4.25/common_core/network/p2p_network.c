#define _GNU_SOURCE

#include "p2p_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>

static P2PConfig g_config = {0};
static P2PNode g_nodes[P2P_MAX_NODES] = {0};
static int g_node_count = 0;
static int g_server_socket = -1;
static int g_discovery_socket = -1;
static pthread_t g_server_thread = 0;
static pthread_t g_discovery_thread = 0;
static pthread_mutex_t g_nodes_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint32_t g_sequence = 0;
static bool g_initialized = false;

static uint64_t get_current_timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static uint32_t get_local_ip_address(void) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return 0;

    struct sockaddr_in google_dns;
    memset(&google_dns, 0, sizeof(google_dns));
    google_dns.sin_family = AF_INET;
    google_dns.sin_addr.s_addr = inet_addr("8.8.8.8");
    google_dns.sin_port = htons(53);

    if (connect(sock, (struct sockaddr*)&google_dns, sizeof(google_dns)) < 0) {
        close(sock);
        return 0;
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sock, (struct sockaddr*)&local_addr, &addr_len) < 0) {
        close(sock);
        return 0;
    }

    close(sock);
    return local_addr.sin_addr.s_addr;
}

static P2PNode* find_node_by_uid(const char* uid) {
    for (int i = 0; i < g_node_count; i++) {
        if (strcmp(g_nodes[i].uid, uid) == 0) {
            return &g_nodes[i];
        }
    }
    return NULL;
}

static P2PNode* find_empty_node_slot(void) {
    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] == '\0') {
            return &g_nodes[i];
        }
    }
    return NULL;
}

int p2p_network_init(P2PConfig* config) {
    if (g_initialized) {
        return 0;
    }

    if (!config || !config->local_uid || config->local_uid[0] == '\0') {
        fprintf(stderr, "[P2P] Error: Invalid config\n");
        return -1;
    }

    memcpy(&g_config, config, sizeof(P2PConfig));
    g_config.local_ip = get_local_ip_address();

    if (g_config.listen_port == 0) {
        g_config.listen_port = P2P_PORT;
    }

    g_initialized = true;
    printf("[P2P] Initialized: UID=%s, IP=%s, Port=%d\n",
           g_config.local_uid,
           inet_ntoa(*(struct in_addr*)&g_config.local_ip),
           g_config.listen_port);

    return 0;
}

void p2p_network_destroy(void) {
    if (!g_initialized) {
        return;
    }

    p2p_stop_discovery();
    p2p_stop_server();
    p2p_disconnect_all_nodes();

    pthread_mutex_lock(&g_nodes_mutex);
    memset(g_nodes, 0, sizeof(g_nodes));
    g_node_count = 0;
    pthread_mutex_unlock(&g_nodes_mutex);

    g_initialized = false;
    printf("[P2P] Destroyed\n");
}

bool p2p_start_server(uint16_t port) {
    if (!g_initialized) {
        return false;
    }

    if (g_server_socket >= 0) {
        printf("[P2P] Server already running on port %d\n", port);
        return true;
    }

    g_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_socket < 0) {
        fprintf(stderr, "[P2P] Failed to create server socket: %s\n", strerror(errno));
        return false;
    }

    int opt = 1;
    setsockopt(g_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(g_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "[P2P] Failed to bind server socket: %s\n", strerror(errno));
        close(g_server_socket);
        g_server_socket = -1;
        return false;
    }

    if (listen(g_server_socket, 10) < 0) {
        fprintf(stderr, "[P2P] Failed to listen: %s\n", strerror(errno));
        close(g_server_socket);
        g_server_socket = -1;
        return false;
    }

    g_config.is_server_running = true;
    g_config.listen_port = port;

    printf("[P2P] Server started on port %d\n", port);
    return true;
}

void p2p_stop_server(void) {
    if (g_server_socket >= 0) {
        close(g_server_socket);
        g_server_socket = -1;
    }
    g_config.is_server_running = false;
    printf("[P2P] Server stopped\n");
}

bool p2p_start_discovery(void) {
    if (!g_initialized) {
        return false;
    }

    if (g_discovery_socket >= 0) {
        printf("[P2P] Discovery already running\n");
        return true;
    }

    g_discovery_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_discovery_socket < 0) {
        fprintf(stderr, "[P2P] Failed to create discovery socket: %s\n", strerror(errno));
        return false;
    }

    int broadcast = 1;
    setsockopt(g_discovery_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    struct sockaddr_in discovery_addr;
    memset(&discovery_addr, 0, sizeof(discovery_addr));
    discovery_addr.sin_family = AF_INET;
    discovery_addr.sin_addr.s_addr = INADDR_ANY;
    discovery_addr.sin_port = htons(P2P_DISCOVERY_PORT);

    if (bind(g_discovery_socket, (struct sockaddr*)&discovery_addr, sizeof(discovery_addr)) < 0) {
        fprintf(stderr, "[P2P] Failed to bind discovery socket: %s\n", strerror(errno));
        close(g_discovery_socket);
        g_discovery_socket = -1;
        return false;
    }

    g_config.is_discovery_running = true;
    printf("[P2P] Discovery started on port %d\n", P2P_DISCOVERY_PORT);
    return true;
}

void p2p_stop_discovery(void) {
    if (g_discovery_socket >= 0) {
        close(g_discovery_socket);
        g_discovery_socket = -1;
    }
    g_config.is_discovery_running = false;
    printf("[P2P] Discovery stopped\n");
}

int p2p_connect_to_node(const char* uid, uint32_t ip, uint16_t port) {
    if (!g_initialized || !uid) {
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "[P2P] Failed to create socket for %s: %s\n", uid, strerror(errno));
        return -1;
    }

    struct sockaddr_in node_addr;
    memset(&node_addr, 0, sizeof(node_addr));
    node_addr.sin_family = AF_INET;
    node_addr.sin_addr.s_addr = ip;
    node_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&node_addr, sizeof(node_addr)) < 0) {
        fprintf(stderr, "[P2P] Failed to connect to %s (%s:%d): %s\n",
                uid, inet_ntoa(*(struct in_addr*)&ip), port, strerror(errno));
        close(sock);
        return -1;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    P2PNode* node = find_node_by_uid(uid);
    if (!node) {
        node = find_empty_node_slot();
        if (node) {
            memset(node, 0, sizeof(P2PNode));
            strncpy(node->uid, uid, sizeof(node->uid) - 1);
            g_node_count++;
        }
    }

    if (node) {
        node->ip_addr = ip;
        node->port = port;
        node->status = P2P_NODE_STATUS_CONNECTED;
        node->last_heartbeat = get_current_timestamp();

        printf("[P2P] Connected to %s (%s:%d)\n",
               uid, inet_ntoa(*(struct in_addr*)&ip), port);

        if (g_config.connection_changed_callback) {
            g_config.connection_changed_callback(uid, true, g_config.callback_user_data);
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);

    close(sock);
    return 0;
}

void p2p_disconnect_from_node(const char* uid) {
    if (!g_initialized || !uid) {
        return;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    P2PNode* node = find_node_by_uid(uid);
    if (node) {
        node->status = P2P_NODE_STATUS_OFFLINE;
        node->last_seen = get_current_timestamp();

        printf("[P2P] Disconnected from %s\n", uid);

        if (g_config.connection_changed_callback) {
            g_config.connection_changed_callback(uid, false, g_config.callback_user_data);
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);
}

void p2p_disconnect_all_nodes(void) {
    pthread_mutex_lock(&g_nodes_mutex);

    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' && g_nodes[i].status == P2P_NODE_STATUS_CONNECTED) {
            p2p_disconnect_from_node(g_nodes[i].uid);
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);
}

int p2p_send_message(const char* to_uid, P2PMessage* message) {
    if (!g_initialized || !to_uid || !message) {
        return -1;
    }

    P2PNode* node = find_node_by_uid(to_uid);
    if (!node || node->status != P2P_NODE_STATUS_CONNECTED) {
        fprintf(stderr, "[P2P] Node %s is not connected\n", to_uid);
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    struct sockaddr_in node_addr;
    memset(&node_addr, 0, sizeof(node_addr));
    node_addr.sin_family = AF_INET;
    node_addr.sin_addr.s_addr = node->ip_addr;
    node_addr.sin_port = htons(node->port);

    if (connect(sock, (struct sockaddr*)&node_addr, sizeof(node_addr)) < 0) {
        close(sock);
        return -1;
    }

    message->sequence = ++g_sequence;
    message->timestamp = get_current_timestamp();
    strncpy(message->from_uid, g_config.local_uid, sizeof(message->from_uid) - 1);
    strncpy(message->to_uid, to_uid, sizeof(message->to_uid) - 1);
    message->from_ip = g_config.local_ip;

    ssize_t sent = send(sock, message, sizeof(P2PMessage), 0);
    close(sock);

    if (sent > 0) {
        printf("[P2P] Message sent to %s: type=%d, seq=%u\n", to_uid, message->type, message->sequence);
        return 0;
    }

    return -1;
}

int p2p_broadcast_message(P2PMessage* message) {
    if (!g_initialized || !message) {
        return -1;
    }

    int success_count = 0;
    pthread_mutex_lock(&g_nodes_mutex);

    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' &&
            g_nodes[i].status == P2P_NODE_STATUS_CONNECTED &&
            g_nodes[i].is_friend) {
            if (p2p_send_message(g_nodes[i].uid, message) == 0) {
                success_count++;
            }
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);

    return success_count;
}

P2PNode* p2p_get_node_by_uid(const char* uid) {
    if (!g_initialized || !uid) {
        return NULL;
    }

    pthread_mutex_lock(&g_nodes_mutex);
    P2PNode* node = find_node_by_uid(uid);
    pthread_mutex_unlock(&g_nodes_mutex);

    return node;
}

P2PNode* p2p_get_friend_node(int index) {
    if (!g_initialized || index < 0) {
        return NULL;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    int count = 0;
    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' && g_nodes[i].is_friend) {
            if (count == index) {
                pthread_mutex_unlock(&g_nodes_mutex);
                return &g_nodes[i];
            }
            count++;
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);
    return NULL;
}

int p2p_get_online_friend_count(void) {
    if (!g_initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    int count = 0;
    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' && g_nodes[i].is_friend &&
            g_nodes[i].status == P2P_NODE_STATUS_CONNECTED) {
            count++;
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);
    return count;
}

int p2p_get_all_nodes_count(void) {
    if (!g_initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_nodes_mutex);
    int count = g_node_count;
    pthread_mutex_unlock(&g_nodes_mutex);

    return count;
}

bool p2p_add_friend(const char* uid) {
    if (!g_initialized || !uid) {
        return false;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    P2PNode* node = find_node_by_uid(uid);
    if (node) {
        node->is_friend = true;
        printf("[P2P] Added friend: %s\n", uid);
        pthread_mutex_unlock(&g_nodes_mutex);
        return true;
    }

    node = find_empty_node_slot();
    if (node) {
        memset(node, 0, sizeof(P2PNode));
        strncpy(node->uid, uid, sizeof(node->uid) - 1);
        node->is_friend = true;
        node->status = P2P_NODE_STATUS_OFFLINE;
        g_node_count++;
        printf("[P2P] Added friend (new node): %s\n", uid);
        pthread_mutex_unlock(&g_nodes_mutex);
        return true;
    }

    pthread_mutex_unlock(&g_nodes_mutex);
    return false;
}

bool p2p_remove_friend(const char* uid) {
    if (!g_initialized || !uid) {
        return false;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    P2PNode* node = find_node_by_uid(uid);
    if (node) {
        node->is_friend = false;
        printf("[P2P] Removed friend: %s\n", uid);
        pthread_mutex_unlock(&g_nodes_mutex);
        return true;
    }

    pthread_mutex_unlock(&g_nodes_mutex);
    return false;
}

bool p2p_is_friend(const char* uid) {
    if (!g_initialized || !uid) {
        return false;
    }

    P2PNode* node = find_node_by_uid(uid);
    return node && node->is_friend;
}

void p2p_update_location(double latitude, double longitude) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_nodes_mutex);

    P2PNode* self = find_node_by_uid(g_config.local_uid);
    if (self) {
        self->latitude = latitude;
        self->longitude = longitude;
        self->last_seen = get_current_timestamp();
    }

    pthread_mutex_unlock(&g_nodes_mutex);

    printf("[P2P] Updated location: %.6f, %.6f\n", latitude, longitude);
}

int p2p_send_location_to_friend(const char* friend_uid, double latitude, double longitude) {
    if (!g_initialized || !friend_uid) {
        return -1;
    }

    if (!p2p_is_friend(friend_uid)) {
        fprintf(stderr, "[P2P] Cannot send location to non-friend: %s\n", friend_uid);
        return -1;
    }

    P2PMessage message;
    memset(&message, 0, sizeof(message));
    message.type = P2P_MESSAGE_TYPE_LOCATION;
    message.payload_size = sizeof(double) * 2;
    memcpy(message.payload, &latitude, sizeof(double));
    memcpy(message.payload + sizeof(double), &longitude, sizeof(double));

    return p2p_send_message(friend_uid, &message);
}

const char* p2p_get_local_uid(void) {
    return g_initialized ? g_config.local_uid : NULL;
}

uint32_t p2p_get_local_ip(void) {
    return g_config.local_ip;
}

uint16_t p2p_get_listen_port(void) {
    return g_config.listen_port;
}

bool p2p_is_connected_to(const char* uid) {
    if (!g_initialized || !uid) {
        return false;
    }

    P2PNode* node = find_node_by_uid(uid);
    return node && node->status == P2P_NODE_STATUS_CONNECTED;
}

bool p2p_is_server_running(void) {
    return g_initialized && g_config.is_server_running;
}

bool p2p_is_discovery_running(void) {
    return g_initialized && g_config.is_discovery_running;
}

void p2p_set_message_callback(P2PMessageCallback callback, void* user_data) {
    if (g_initialized) {
        g_config.message_received_callback = callback;
        g_config.callback_user_data = user_data;
    }
}

void p2p_set_node_callback(P2PNodeCallback callback, void* user_data) {
    if (g_initialized) {
        g_config.node_discovered_callback = callback;
        g_config.callback_user_data = user_data;
    }
}

void p2p_set_connection_callback(P2PConnectionCallback callback, void* user_data) {
    if (g_initialized) {
        g_config.connection_changed_callback = callback;
        g_config.callback_user_data = user_data;
    }
}

const char* p2p_get_node_status_string(P2PNodeStatus status) {
    switch (status) {
        case P2P_NODE_STATUS_OFFLINE: return "OFFLINE";
        case P2P_NODE_STATUS_ONLINE: return "ONLINE";
        case P2P_NODE_STATUS_CONNECTING: return "CONNECTING";
        case P2P_NODE_STATUS_CONNECTED: return "CONNECTED";
        default: return "UNKNOWN";
    }
}

void p2p_dump_nodes(void) {
    if (!g_initialized) {
        return;
    }

    printf("\n=== P2P Nodes (%d total) ===\n", g_node_count);
    pthread_mutex_lock(&g_nodes_mutex);

    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0') {
            printf("  [%s] %s - %s:%d - %s - Friend: %s\n",
                   g_nodes[i].uid,
                   p2p_get_node_status_string(g_nodes[i].status),
                   inet_ntoa(*(struct in_addr*)&g_nodes[i].ip_addr),
                   g_nodes[i].port,
                   g_nodes[i].nickname[0] ? g_nodes[i].nickname : "(no nickname)",
                   g_nodes[i].is_friend ? "YES" : "NO");
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);
    printf("==========================\n\n");
}

void p2p_dump_friends(void) {
    if (!g_initialized) {
        return;
    }

    printf("\n=== P2P Friends ===\n");
    pthread_mutex_lock(&g_nodes_mutex);

    int count = 0;
    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' && g_nodes[i].is_friend) {
            printf("  [%d] %s - %s - Location: %.4f, %.4f\n",
                   count + 1,
                   g_nodes[i].uid,
                   p2p_get_node_status_string(g_nodes[i].status),
                   g_nodes[i].latitude, g_nodes[i].longitude);
            count++;
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);
    printf("==================\n  Total: %d friends\n\n", count);
}

int p2p_heartbeat(void) {
    if (!g_initialized) {
        return -1;
    }

    P2PMessage message;
    memset(&message, 0, sizeof(message));
    message.type = P2P_MESSAGE_TYPE_HEARTBEAT;

    int sent_count = 0;
    pthread_mutex_lock(&g_nodes_mutex);

    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' && g_nodes[i].status == P2P_NODE_STATUS_CONNECTED) {
            if (p2p_send_message(g_nodes[i].uid, &message) == 0) {
                sent_count++;
            }
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);

    return sent_count;
}

int p2p_cleanup_stale_connections(uint64_t timeout_ms) {
    if (!g_initialized) {
        return 0;
    }

    uint64_t now = get_current_timestamp();
    int cleaned = 0;

    pthread_mutex_lock(&g_nodes_mutex);

    for (int i = 0; i < P2P_MAX_NODES; i++) {
        if (g_nodes[i].uid[0] != '\0' &&
            g_nodes[i].status == P2P_NODE_STATUS_CONNECTED &&
            (now - g_nodes[i].last_heartbeat) > timeout_ms) {

            g_nodes[i].status = P2P_NODE_STATUS_OFFLINE;
            printf("[P2P] Stale connection cleaned: %s\n", g_nodes[i].uid);
            cleaned++;

            if (g_config.connection_changed_callback) {
                g_config.connection_changed_callback(g_nodes[i].uid, false,
                                                    g_config.callback_user_data);
            }
        }
    }

    pthread_mutex_unlock(&g_nodes_mutex);

    return cleaned;
}
