// FindFriend P2P 接口适配层

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

static bool g_p2p_running = false;
static uint8_t g_self_node_id[32];
static pthread_mutex_t g_p2p_lock;

extern FFResult ff_p2p_node_discovery_init(const uint8_t* node_id);
extern void ff_p2p_node_discovery_cleanup(void);
extern FFResult ff_p2p_node_discovery_start(void);
extern FFResult ff_p2p_node_add(const uint8_t* node_id, const char* ip, uint16_t port, uint32_t uid);
extern FFResult ff_p2p_node_remove(const uint8_t* node_id);
extern FFResult ff_p2p_node_get(const uint8_t* node_id, P2PNode* info);
extern FFResult ff_p2p_node_get_all(P2PNode** nodes, size_t* count);
extern bool ff_p2p_node_exists(const uint8_t* node_id);
extern void ff_p2p_node_cleanup(void);

extern FFResult ff_p2p_connection_init(void);
extern void ff_p2p_connection_cleanup(void);
extern FFResult ff_p2p_connection_connect(const uint8_t* node_id, const char* ip, uint16_t port);
extern FFResult ff_p2p_connection_disconnect(const uint8_t* node_id);
extern FFResult ff_p2p_connection_send(uint32_t conn_id, const uint8_t* data, size_t length);
extern int ff_p2p_connection_get_count(void);
extern int ff_p2p_connection_get_connected_count(void);

extern FFResult ff_p2p_message_routing_init(void);
extern void ff_p2p_message_routing_cleanup(void);

extern FFResult ff_p2p_nat_traversal_init(void);
extern void ff_p2p_nat_traversal_cleanup(void);

FFResult ff_p2p_init(FFAsyncCallback callback, void* user_data) {
    pthread_mutex_init(&g_p2p_lock, NULL);

    // 生成随机节点 ID
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 32; i++) {
        g_self_node_id[i] = (uint8_t)(rand() % 256);
    }

    FFResult res;

    res = ff_p2p_node_discovery_init(g_self_node_id);
    if (res != FF_OK) {
        return res;
    }

    res = ff_p2p_connection_init();
    if (res != FF_OK) {
        ff_p2p_node_discovery_cleanup();
        return res;
    }

    res = ff_p2p_message_routing_init();
    if (res != FF_OK) {
        ff_p2p_connection_cleanup();
        ff_p2p_node_discovery_cleanup();
        return res;
    }

    res = ff_p2p_nat_traversal_init();
    if (res != FF_OK) {
        ff_p2p_message_routing_cleanup();
        ff_p2p_connection_cleanup();
        ff_p2p_node_discovery_cleanup();
        return res;
    }

    g_p2p_running = true;
    FF_LOG_INFO("P2P module initialized");

    if (callback) {
        callback(FF_OK, user_data);
    }

    return FF_OK;
}

FFResult ff_p2p_shutdown(void) {
    if (!g_p2p_running) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    g_p2p_running = false;

    ff_p2p_nat_traversal_cleanup();
    ff_p2p_message_routing_cleanup();
    ff_p2p_connection_cleanup();
    ff_p2p_node_discovery_cleanup();

    pthread_mutex_destroy(&g_p2p_lock);

    FF_LOG_INFO("P2P module shutdown");
    return FF_OK;
}

bool ff_p2p_is_running(void) {
    return g_p2p_running;
}

FFResult ff_p2p_start_node(FFAsyncCallback callback, void* user_data) {
    if (!g_p2p_running) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    FFResult res = ff_p2p_node_discovery_start();
    if (res != FF_OK) {
        return res;
    }

    FF_LOG_INFO("P2P node started");

    if (callback) {
        callback(FF_OK, user_data);
    }

    return FF_OK;
}

FFResult ff_p2p_stop_node(void) {
    ff_p2p_node_cleanup();
    FF_LOG_INFO("P2P node stopped");
    return FF_OK;
}

FFResult ff_p2p_get_node_info(P2PNode** node_info) {
    if (!node_info) {
        return FF_ERROR_INVALID_PARAM;
    }

    *node_info = (P2PNode*)malloc(sizeof(P2PNode));
    if (!*node_info) {
        return FF_ERROR_NO_MEMORY;
    }

    memset(*node_info, 0, sizeof(P2PNode));
    memcpy((*node_info)->node_id, g_self_node_id, 32);
    strncpy((*node_info)->public_ip, "0.0.0.0", sizeof((*node_info)->public_ip) - 1);
    (*node_info)->public_port = 0;
    (*node_info)->uid = 1000;
    (*node_info)->status = g_p2p_running ? 1 : 0;
    (*node_info)->last_seen = time(NULL);

    return FF_OK;
}

FFResult ff_p2p_start_discovery(FFAsyncCallback callback, void* user_data) {
    if (!g_p2p_running) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    FF_LOG_INFO("P2P discovery started");

    if (callback) {
        callback(FF_OK, user_data);
    }

    return FF_OK;
}

FFResult ff_p2p_stop_discovery(void) {
    FF_LOG_INFO("P2P discovery stopped");
    return FF_OK;
}

FFResult ff_p2p_search_nodes(const uint8_t* criteria, size_t criteria_len, P2PNode** nodes, size_t* count, FFAsyncCallback callback, void* user_data) {
    return ff_p2p_node_get_all(nodes, count);
}

FFResult ff_p2p_get_known_nodes(P2PNode** nodes, size_t* count) {
    return ff_p2p_node_get_all(nodes, count);
}

FFResult ff_p2p_connect(const uint8_t* peer_id, FFAsyncCallback callback, void* user_data) {
    (void)peer_id;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_disconnect(const uint8_t* peer_id) {
    return ff_p2p_node_remove(peer_id);
}

FFResult ff_p2p_get_connection_state(const uint8_t* peer_id, P2PConnectionState* state) {
    (void)peer_id;
    (void)state;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_get_active_connections(P2PConnection** connections, size_t* count) {
    (void)connections;
    (void)count;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_send(const uint8_t* peer_id, const void* data, size_t data_len, FFAsyncCallback callback, void* user_data) {
    (void)peer_id;
    (void)data;
    (void)data_len;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_broadcast(const void* data, size_t data_len, FFAsyncCallback callback, void* user_data) {
    (void)data;
    (void)data_len;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_receive(const uint8_t* peer_id, void* buffer, size_t buffer_size, size_t* received, FFAsyncCallback callback, void* user_data) {
    (void)peer_id;
    (void)buffer;
    (void)buffer_size;
    (void)received;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_detect_nat_type(P2PNATType* nat_type, FFAsyncCallback callback, void* user_data) {
    if (nat_type) {
        *nat_type = P2P_NAT_FULL_CONE;
    }
    if (callback) {
        callback(FF_OK, user_data);
    }
    return FF_OK;
}

FFResult ff_p2p_hole_punch(const uint8_t* target_peer_id, FFAsyncCallback callback, void* user_data) {
    (void)target_peer_id;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_set_stun_server(const char* stun_host, uint16_t stun_port) {
    (void)stun_host;
    (void)stun_port;
    return FF_OK;
}

FFResult ff_p2p_get_external_address(char* ip, uint16_t* port) {
    if (ip) strncpy(ip, "0.0.0.0", 16);
    if (port) *port = 0;
    return FF_OK;
}

FFResult ff_p2p_connect_friend(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    (void)uid;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_send_to_friend(uint32_t uid, const void* data, size_t data_len, FFAsyncCallback callback, void* user_data) {
    (void)uid;
    (void)data;
    (void)data_len;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_receive_from_friend(uint32_t uid, void* buffer, size_t buffer_size, size_t* received, FFAsyncCallback callback, void* user_data) {
    (void)uid;
    (void)buffer;
    (void)buffer_size;
    (void)received;
    (void)callback;
    (void)user_data;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_update_reputation(const uint8_t* peer_id, int32_t score_delta) {
    (void)peer_id;
    (void)score_delta;
    return FF_ERROR_UNSUPPORTED;
}

FFResult ff_p2p_get_reputation(const uint8_t* peer_id, int32_t* score) {
    (void)peer_id;
    if (score) *score = 0;
    return FF_OK;
}

void ff_p2p_node_free(P2PNode* node) {
    if (node) {
        free(node);
    }
}

void ff_p2p_node_list_free(P2PNode* nodes, size_t count) {
    (void)count;
    if (nodes) {
        free(nodes);
    }
}

void ff_p2p_connection_free(P2PConnection* connections, size_t count) {
    (void)count;
    if (connections) {
        free(connections);
    }
}