// FindFriend P2P 连接模块

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>

#define MAX_CONNECTIONS 1024
#define CONNECT_TIMEOUT 5 // 秒

// 连接状态
typedef enum {
    FF_CONN_STATUS_DISCONNECTED = 0,
    FF_CONN_STATUS_CONNECTING = 1,
    FF_CONN_STATUS_CONNECTED = 2,
    FF_CONN_STATUS_ERROR = 3
} FFConnectionStatus;

// 连接信息结构
typedef struct {
    uint8_t node_id[32];
    int socket_fd;
    FFConnectionStatus status;
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
    uint64_t last_activity;
    pthread_mutex_t lock;
} Connection;

static Connection g_connections[MAX_CONNECTIONS];
static size_t g_connection_count = 0;
static pthread_mutex_t g_connections_lock;

FFResult ff_p2p_connection_init(void) {
    g_connection_count = 0;
    memset(g_connections, 0, sizeof(g_connections));
    
    // 初始化连接锁
    pthread_mutex_init(&g_connections_lock, NULL);
    
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        pthread_mutex_init(&g_connections[i].lock, NULL);
    }
    FF_LOG_INFO("Connection manager initialized");
    return FF_OK;
}

void ff_p2p_connection_cleanup(void) {
    for (size_t i = 0; i < g_connection_count; i++) {
        if (g_connections[i].socket_fd >= 0) {
            close(g_connections[i].socket_fd);
        }
        pthread_mutex_destroy(&g_connections[i].lock);
    }
    
    // 销毁连接锁
    pthread_mutex_destroy(&g_connections_lock);
    
    g_connection_count = 0;
    FF_LOG_INFO("Connection manager cleanup");
}

FFResult ff_p2p_connection_create(const uint8_t* node_id, const char* ip, uint16_t port, uint32_t* conn_id) {
    if (!node_id || !ip || !conn_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    pthread_mutex_lock(&g_connections_lock);
    
    if (g_connection_count >= MAX_CONNECTIONS) {
        pthread_mutex_unlock(&g_connections_lock);
        return FF_ERROR_NO_MEMORY;
    }
    
    // 检查是否已存在连接
    for (size_t i = 0; i < g_connection_count; i++) {
        if (memcmp(g_connections[i].node_id, node_id, 32) == 0) {
            pthread_mutex_unlock(&g_connections_lock);
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    Connection* conn = &g_connections[g_connection_count];
    memcpy(conn->node_id, node_id, 32);
    strncpy(conn->ip, ip, INET_ADDRSTRLEN - 1);
    conn->port = port;
    conn->socket_fd = -1;
    conn->status = FF_CONN_STATUS_DISCONNECTED;
    conn->last_activity = 0;
    
    *conn_id = g_connection_count;
    g_connection_count++;
    
    pthread_mutex_unlock(&g_connections_lock);
    FF_LOG_INFO("Connection created for node: %s:%u", ip, port);
    return FF_OK;
}

FFResult ff_p2p_connection_connect(uint32_t conn_id) {
    if (conn_id >= g_connection_count) {
        return FF_ERROR_NOT_FOUND;
    }
    
    Connection* conn = &g_connections[conn_id];
    pthread_mutex_lock(&conn->lock);
    
    if (conn->status == FF_CONN_STATUS_CONNECTED) {
        pthread_mutex_unlock(&conn->lock);
        return FF_OK;
    }
    
    conn->status = FF_CONN_STATUS_CONNECTING;
    
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        conn->status = FF_CONN_STATUS_ERROR;
        pthread_mutex_unlock(&conn->lock);
        FF_LOG_ERROR("Failed to create socket");
        return FF_ERROR_UNKNOWN;
    }
    
    // 设置超时
    struct timeval timeout;
    timeout.tv_sec = CONNECT_TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    // 连接到节点
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(conn->ip);
    addr.sin_port = htons(conn->port);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        conn->status = FF_CONN_STATUS_ERROR;
        pthread_mutex_unlock(&conn->lock);
        FF_LOG_ERROR("Failed to connect to %s:%u", conn->ip, conn->port);
        return FF_ERROR_UNKNOWN;
    }
    
    conn->socket_fd = sock;
    conn->status = FF_CONN_STATUS_CONNECTED;
    conn->last_activity = time(NULL);
    
    pthread_mutex_unlock(&conn->lock);
    FF_LOG_INFO("Connected to node: %s:%u", conn->ip, conn->port);
    return FF_OK;
}

FFResult ff_p2p_connection_disconnect(uint32_t conn_id) {
    if (conn_id >= g_connection_count) {
        return FF_ERROR_NOT_FOUND;
    }
    
    Connection* conn = &g_connections[conn_id];
    pthread_mutex_lock(&conn->lock);
    
    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
        conn->socket_fd = -1;
    }
    conn->status = FF_CONN_STATUS_DISCONNECTED;
    
    pthread_mutex_unlock(&conn->lock);
    FF_LOG_INFO("Disconnected from node: %s:%u", conn->ip, conn->port);
    return FF_OK;
}

FFResult ff_p2p_connection_send(uint32_t conn_id, const uint8_t* data, size_t length) {
    if (conn_id >= g_connection_count || !data) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    Connection* conn = &g_connections[conn_id];
    pthread_mutex_lock(&conn->lock);
    
    if (conn->status != FF_CONN_STATUS_CONNECTED || conn->socket_fd < 0) {
        pthread_mutex_unlock(&conn->lock);
        return FF_ERROR_NOT_FOUND;
    }
    
    ssize_t sent = send(conn->socket_fd, data, length, 0);
    if (sent < 0) {
        conn->status = FF_CONN_STATUS_ERROR;
        close(conn->socket_fd);
        conn->socket_fd = -1;
        pthread_mutex_unlock(&conn->lock);
        FF_LOG_ERROR("Failed to send data");
        return FF_ERROR_UNKNOWN;
    }
    
    conn->last_activity = time(NULL);
    
    pthread_mutex_unlock(&conn->lock);
    return FF_OK;
}

FFResult ff_p2p_connection_recv(uint32_t conn_id, uint8_t* buffer, size_t buffer_size, size_t* received) {
    if (conn_id >= g_connection_count || !buffer || !received) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    Connection* conn = &g_connections[conn_id];
    pthread_mutex_lock(&conn->lock);
    
    if (conn->status != FF_CONN_STATUS_CONNECTED || conn->socket_fd < 0) {
        pthread_mutex_unlock(&conn->lock);
        return FF_ERROR_NOT_FOUND;
    }
    
    ssize_t recv_len = recv(conn->socket_fd, buffer, buffer_size, 0);
    if (recv_len < 0) {
        conn->status = FF_CONN_STATUS_ERROR;
        close(conn->socket_fd);
        conn->socket_fd = -1;
        pthread_mutex_unlock(&conn->lock);
        FF_LOG_ERROR("Failed to receive data");
        return FF_ERROR_UNKNOWN;
    } else if (recv_len == 0) {
        conn->status = FF_CONN_STATUS_DISCONNECTED;
        close(conn->socket_fd);
        conn->socket_fd = -1;
        pthread_mutex_unlock(&conn->lock);
        return FF_ERROR_NOT_FOUND;
    }
    
    *received = recv_len;
    conn->last_activity = time(NULL);
    
    pthread_mutex_unlock(&conn->lock);
    return FF_OK;
}

FFResult ff_p2p_connection_get_status(uint32_t conn_id, FFConnectionStatus* status) {
    if (conn_id >= g_connection_count || !status) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    Connection* conn = &g_connections[conn_id];
    pthread_mutex_lock(&conn->lock);
    *status = conn->status;
    pthread_mutex_unlock(&conn->lock);
    
    return FF_OK;
}

FFResult ff_p2p_connection_get_info(uint32_t conn_id, P2PConnection* info) {
    if (conn_id >= g_connection_count || !info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    Connection* conn = &g_connections[conn_id];
    pthread_mutex_lock(&conn->lock);
    
    memcpy(info->peer_id, conn->node_id, 32);
    info->state = (P2PConnectionState)conn->status;
    info->connected_time = conn->last_activity;
    info->last_activity = conn->last_activity;
    info->bytes_sent = 0;
    info->bytes_received = 0;
    
    pthread_mutex_unlock(&conn->lock);
    return FF_OK;
}

FFResult ff_p2p_connection_remove(uint32_t conn_id) {
    if (conn_id >= g_connection_count) {
        return FF_ERROR_NOT_FOUND;
    }
    
    pthread_mutex_lock(&g_connections_lock);
    
    Connection* conn = &g_connections[conn_id];
    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
    }
    pthread_mutex_destroy(&conn->lock);
    
    // 移除连接
    memmove(&g_connections[conn_id], &g_connections[conn_id + 1], 
            (g_connection_count - conn_id - 1) * sizeof(Connection));
    g_connection_count--;
    
    pthread_mutex_unlock(&g_connections_lock);
    FF_LOG_INFO("Connection removed");
    return FF_OK;
}

void ff_p2p_connection_cleanup_idle(void) {
    time_t now = time(NULL);
    
    for (size_t i = 0; i < g_connection_count; ) {
        Connection* conn = &g_connections[i];
        pthread_mutex_lock(&conn->lock);
        
        if (conn->status == FF_CONN_STATUS_CONNECTED && now - conn->last_activity > 300) {
            if (conn->socket_fd >= 0) {
                close(conn->socket_fd);
            }
            conn->status = FF_CONN_STATUS_DISCONNECTED;
            conn->socket_fd = -1;
            FF_LOG_INFO("Connection timed out: %s:%u", conn->ip, conn->port);
        }
        
        pthread_mutex_unlock(&conn->lock);
        i++;
    }
}

FFResult ff_p2p_connection_get_by_node_id(const uint8_t* node_id, uint32_t* conn_id) {
    if (!node_id || !conn_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    pthread_mutex_lock(&g_connections_lock);
    
    for (size_t i = 0; i < g_connection_count; i++) {
        if (memcmp(g_connections[i].node_id, node_id, 32) == 0) {
            *conn_id = i;
            pthread_mutex_unlock(&g_connections_lock);
            return FF_OK;
        }
    }
    
    pthread_mutex_unlock(&g_connections_lock);
    return FF_ERROR_NOT_FOUND;
}

size_t ff_p2p_connection_get_count(void) {
    pthread_mutex_lock(&g_connections_lock);
    size_t count = g_connection_count;
    pthread_mutex_unlock(&g_connections_lock);
    return count;
}

size_t ff_p2p_connection_get_connected_count(void) {
    size_t count = 0;
    pthread_mutex_lock(&g_connections_lock);
    
    for (size_t i = 0; i < g_connection_count; i++) {
        Connection* conn = &g_connections[i];
        pthread_mutex_lock(&conn->lock);
        if (conn->status == FF_CONN_STATUS_CONNECTED) {
            count++;
        }
        pthread_mutex_unlock(&conn->lock);
    }
    
    pthread_mutex_unlock(&g_connections_lock);
    return count;
}
