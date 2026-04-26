// FindFriend P2P 网络模块 - 连接管理

#include "ff_p2p_interface.h"
#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool g_initialized = false;
static P2PConnection* g_connections = NULL;
static size_t g_connection_count = 0;
static size_t g_connection_capacity = 0;

// 初始化连接管理
FFResult ff_p2p_connection_init(void) {
    FF_INFO("P2P connection manager initialized");
    g_initialized = true;
    return FF_OK;
}

// 清理连接管理
void ff_p2p_connection_cleanup(void) {
    if (g_initialized) {
        if (g_connections) {
            for (size_t i = 0; i < g_connection_count; i++) {
                // 清理连接
                ff_p2p_disconnect(g_connections[i].peer_id);
            }
            free(g_connections);
            g_connections = NULL;
        }
        g_connection_count = 0;
        g_connection_capacity = 0;
        g_initialized = false;
        FF_INFO("P2P connection manager cleanup");
    }
}

// 创建连接
FFResult ff_p2p_create_connection(const uint8_t* peer_id) {
    if (!peer_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 检查是否已存在连接
    for (size_t i = 0; i < g_connection_count; i++) {
        if (memcmp(g_connections[i].peer_id, peer_id, 32) == 0) {
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    if (g_connection_count >= g_connection_capacity) {
        size_t new_capacity = g_connection_capacity ? g_connection_capacity * 2 : 16;
        P2PConnection* new_connections = realloc(g_connections, new_capacity * sizeof(P2PConnection));
        if (!new_connections) {
            return FF_ERROR_NO_MEMORY;
        }
        g_connections = new_connections;
        g_connection_capacity = new_capacity;
    }
    
    P2PConnection* conn = &g_connections[g_connection_count];
    memcpy(conn->peer_id, peer_id, 32);
    conn->state = P2P_STATE_CONNECTING;
    conn->connected_time = time(NULL);
    conn->last_activity = time(NULL);
    conn->bytes_sent = 0;
    conn->bytes_received = 0;
    
    g_connection_count++;
    
    FF_DEBUG("Creating P2P connection to peer");
    return FF_OK;
}

// 关闭连接
FFResult ff_p2p_close_connection(const uint8_t* peer_id) {
    if (!peer_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_connection_count; i++) {
        if (memcmp(g_connections[i].peer_id, peer_id, 32) == 0) {
            // 关闭连接
            g_connections[i].state = P2P_STATE_DISCONNECTED;
            
            // 移除此连接
            memmove(&g_connections[i], &g_connections[i + 1], (g_connection_count - i - 1) * sizeof(P2PConnection));
            g_connection_count--;
            
            FF_DEBUG("Closed P2P connection");
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

// 查找连接
FFResult ff_p2p_find_connection(const uint8_t* peer_id, P2PConnection** connection) {
    if (!peer_id || !connection) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_connection_count; i++) {
        if (memcmp(g_connections[i].peer_id, peer_id, 32) == 0) {
            *connection = &g_connections[i];
            return FF_OK;
        }
    }
    
    *connection = NULL;
    return FF_ERROR_NOT_FOUND;
}

// 更新连接状态
FFResult ff_p2p_update_connection_state(const uint8_t* peer_id, P2PConnectionState state) {
    P2PConnection* conn;
    FFResult result = ff_p2p_find_connection(peer_id, &conn);
    if (result != FF_OK) {
        return result;
    }
    
    conn->state = state;
    if (state == P2P_STATE_CONNECTED) {
        conn->connected_time = time(NULL);
    }
    conn->last_activity = time(NULL);
    
    return FF_OK;
}

// 记录数据传输
FFResult ff_p2p_record_transfer(const uint8_t* peer_id, size_t bytes_sent, size_t bytes_received) {
    P2PConnection* conn;
    FFResult result = ff_p2p_find_connection(peer_id, &conn);
    if (result != FF_OK) {
        return result;
    }
    
    conn->bytes_sent += bytes_sent;
    conn->bytes_received += bytes_received;
    conn->last_activity = time(NULL);
    
    return FF_OK;
}

// 获取所有连接
FFResult ff_p2p_get_all_connections(P2PConnection** connections, size_t* count) {
    if (!connections || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *connections = g_connections;
    *count = g_connection_count;
    return FF_OK;
}

// 清理无效连接
FFResult ff_p2p_cleanup_invalid_connections(void) {
    size_t i = 0;
    
    while (i < g_connection_count) {
        if (g_connections[i].state == P2P_STATE_ERROR || 
            g_connections[i].state == P2P_STATE_DISCONNECTED) {
            // 移除此连接
            memmove(&g_connections[i], &g_connections[i + 1], (g_connection_count - i - 1) * sizeof(P2PConnection));
            g_connection_count--;
            FF_DEBUG("Cleaned up invalid P2P connection");
        } else {
            i++;
        }
    }
    
    return FF_OK;
}
