// FindFriend P2P 网络模块 - 节点发现

#include "ff_p2p_interface.h"
#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool g_initialized = false;
static P2PNode* g_nodes = NULL;
static size_t g_node_count = 0;
static size_t g_node_capacity = 0;

// 初始化节点发现
FFResult ff_p2p_node_discovery_init(void) {
    FF_INFO("P2P node discovery initialized");
    g_initialized = true;
    return FF_OK;
}

// 清理节点发现
void ff_p2p_node_discovery_cleanup(void) {
    if (g_initialized) {
        if (g_nodes) {
            for (size_t i = 0; i < g_node_count; i++) {
                // 清理节点数据
            }
            free(g_nodes);
            g_nodes = NULL;
        }
        g_node_count = 0;
        g_node_capacity = 0;
        g_initialized = false;
        FF_INFO("P2P node discovery cleanup");
    }
}

// 添加节点
FFResult ff_p2p_node_add(const P2PNode* node) {
    if (!node) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_node_count >= g_node_capacity) {
        size_t new_capacity = g_node_capacity ? g_node_capacity * 2 : 16;
        P2PNode* new_nodes = realloc(g_nodes, new_capacity * sizeof(P2PNode));
        if (!new_nodes) {
            return FF_ERROR_NO_MEMORY;
        }
        g_nodes = new_nodes;
        g_node_capacity = new_capacity;
    }
    
    g_nodes[g_node_count] = *node;
    g_node_count++;
    
    FF_DEBUG("Added P2P node: %s:%d", node->public_ip, node->public_port);
    return FF_OK;
}

// 移除节点
FFResult ff_p2p_node_remove(const uint8_t* node_id) {
    if (!node_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_node_count; i++) {
        if (memcmp(g_nodes[i].node_id, node_id, 32) == 0) {
            // 移除此节点
            memmove(&g_nodes[i], &g_nodes[i + 1], (g_node_count - i - 1) * sizeof(P2PNode));
            g_node_count--;
            FF_DEBUG("Removed P2P node");
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

// 查找节点
FFResult ff_p2p_node_find(const uint8_t* node_id, P2PNode** node) {
    if (!node_id || !node) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_node_count; i++) {
        if (memcmp(g_nodes[i].node_id, node_id, 32) == 0) {
            *node = &g_nodes[i];
            return FF_OK;
        }
    }
    
    *node = NULL;
    return FF_ERROR_NOT_FOUND;
}

// 获取所有节点
FFResult ff_p2p_get_all_nodes(P2PNode** nodes, size_t* count) {
    if (!nodes || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *nodes = g_nodes;
    *count = g_node_count;
    return FF_OK;
}

// 广播节点发现消息
FFResult ff_p2p_broadcast_discovery(void) {
    FF_INFO("P2P broadcasting discovery message");
    // 实现广播逻辑
    return FF_OK;
}

// 处理发现的节点
FFResult ff_p2p_handle_discovered_node(const P2PNode* node) {
    if (!node) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 检查是否已存在
    P2PNode* existing;
    FFResult result = ff_p2p_node_find(node->node_id, &existing);
    
    if (result == FF_ERROR_NOT_FOUND) {
        // 添加新节点
        return ff_p2p_node_add(node);
    } else if (result == FF_OK) {
        // 更新现有节点
        existing->last_seen = time(NULL);
        existing->status = 1; // online
        return FF_OK;
    }
    
    return result;
}

// 清理过期节点
FFResult ff_p2p_cleanup_expired_nodes(uint64_t timeout_seconds) {
    uint64_t now = time(NULL);
    size_t i = 0;
    
    while (i < g_node_count) {
        if (now - g_nodes[i].last_seen > timeout_seconds) {
            // 移除此节点
            memmove(&g_nodes[i], &g_nodes[i + 1], (g_node_count - i - 1) * sizeof(P2PNode));
            g_node_count--;
            FF_DEBUG("Cleaned up expired P2P node");
        } else {
            i++;
        }
    }
    
    return FF_OK;
}
