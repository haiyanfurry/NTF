// FindFriend P2P 节点发现模块

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

#define MAX_NODES 1024
#define BROADCAST_PORT 5000
#define DISCOVERY_INTERVAL 5 // 秒

// 节点信息结构
typedef struct {
    uint8_t node_id[32];
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
    uint32_t uid;
    uint64_t last_seen;
    bool online;
} NodeInfo;

static NodeInfo g_nodes[MAX_NODES];
static size_t g_node_count = 0;
static int g_broadcast_fd = -1;
static uint8_t g_self_node_id[32];
static bool g_discovery_running = false;

// 函数声明
void* ff_p2p_node_discovery_thread(void* arg);
FFResult ff_p2p_node_add(const uint8_t* node_id, const char* ip, uint16_t port, uint32_t uid);
void ff_p2p_node_cleanup(void);

FFResult ff_p2p_node_discovery_init(const uint8_t* node_id) {
    if (!node_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 初始化节点列表
    g_node_count = 0;
    memset(g_nodes, 0, sizeof(g_nodes));
    memcpy(g_self_node_id, node_id, 32);
    
    // 创建广播套接字
    g_broadcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_broadcast_fd < 0) {
        FF_LOG_ERROR("Failed to create broadcast socket");
        return FF_ERROR_UNKNOWN;
    }
    
    // 设置广播选项
    int broadcast = 1;
    if (setsockopt(g_broadcast_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        FF_LOG_ERROR("Failed to set broadcast option");
        close(g_broadcast_fd);
        g_broadcast_fd = -1;
        return FF_ERROR_UNKNOWN;
    }
    
    // 设置地址重用选项，允许多个节点绑定到同一端口
    int reuseaddr = 1;
    if (setsockopt(g_broadcast_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0) {
        FF_LOG_ERROR("Failed to set reuseaddr option");
        close(g_broadcast_fd);
        g_broadcast_fd = -1;
        return FF_ERROR_UNKNOWN;
    }
    
    // 绑定端口
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(BROADCAST_PORT);
    
    if (bind(g_broadcast_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        FF_LOG_ERROR("Failed to bind broadcast socket");
        close(g_broadcast_fd);
        g_broadcast_fd = -1;
        return FF_ERROR_UNKNOWN;
    }
    
    g_discovery_running = true;
    FF_LOG_INFO("Node discovery initialized");
    return FF_OK;
}

void ff_p2p_node_discovery_cleanup(void) {
    g_discovery_running = false;
    if (g_broadcast_fd >= 0) {
        close(g_broadcast_fd);
        g_broadcast_fd = -1;
    }
    g_node_count = 0;
    FF_LOG_INFO("Node discovery cleanup");
}

FFResult ff_p2p_node_discovery_start(void) {
    if (!g_discovery_running) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    // 启动发现线程
    pthread_t thread;
    if (pthread_create(&thread, NULL, ff_p2p_node_discovery_thread, NULL) != 0) {
        FF_LOG_ERROR("Failed to start discovery thread");
        return FF_ERROR_UNKNOWN;
    }
    
    // 分离线程
    pthread_detach(thread);
    FF_LOG_INFO("Node discovery started");
    return FF_OK;
}

void* ff_p2p_node_discovery_thread(void* arg) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char buffer[512];
    
    char node_id_str[17];
    for (int i = 0; i < 8 && i < 32; i++) {
        sprintf(&node_id_str[i*2], "%02x", g_self_node_id[i]);
    }
    node_id_str[16] = '\0';
    
    FF_LOG_INFO("Node %s trying to broadcast...", node_id_str);
    
    while (g_discovery_running) {
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, g_self_node_id, 32);
        
        struct sockaddr_in broadcast_addr;
        memset(&broadcast_addr, 0, sizeof(broadcast_addr));
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        broadcast_addr.sin_port = htons(BROADCAST_PORT);
        
        ssize_t sent = sendto(g_broadcast_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        if (sent > 0) {
            FF_LOG_INFO("Node %s broadcast sent successfully, %zd bytes", node_id_str, sent);
        } else {
            FF_LOG_ERROR("Node %s broadcast send failed", node_id_str);
        }
        
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(g_broadcast_fd, &read_fds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ready = select(g_broadcast_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ready > 0 && FD_ISSET(g_broadcast_fd, &read_fds)) {
            ssize_t len = recvfrom(g_broadcast_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
            if (len > 0) {
                FF_LOG_INFO("Node %s received %zd bytes from %s:%d", node_id_str, len, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            }
            if (len > 32) {
                uint8_t node_id[32];
                memcpy(node_id, buffer, 32);
                
                if (memcmp(node_id, g_self_node_id, 32) == 0) {
                    FF_LOG_INFO("Node %s skipped own broadcast", node_id_str);
                    continue;
                }
                
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
                uint16_t port = ntohs(addr.sin_port);
                
                FF_LOG_INFO("Node %s discovered new node from %s:%u", node_id_str, ip, port);
                
                ff_p2p_node_add(node_id, ip, port, 0);
            }
        }
        
        ff_p2p_node_cleanup();
        
        FF_LOG_INFO("Node %s current known nodes count: %zu", node_id_str, g_node_count);
        
        sleep(DISCOVERY_INTERVAL);
    }
    
    return NULL;
}

FFResult ff_p2p_node_add(const uint8_t* node_id, const char* ip, uint16_t port, uint32_t uid) {
    if (!node_id || !ip) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 查找现有节点
    for (size_t i = 0; i < g_node_count; i++) {
        if (memcmp(g_nodes[i].node_id, node_id, 32) == 0) {
            // 更新节点信息
            strncpy(g_nodes[i].ip, ip, INET_ADDRSTRLEN - 1);
            g_nodes[i].port = port;
            g_nodes[i].uid = uid;
            g_nodes[i].last_seen = time(NULL);
            g_nodes[i].online = true;
            return FF_OK;
        }
    }
    
    // 添加新节点
    if (g_node_count >= MAX_NODES) {
        return FF_ERROR_NO_MEMORY;
    }
    
    NodeInfo* node = &g_nodes[g_node_count];
    memcpy(node->node_id, node_id, 32);
    strncpy(node->ip, ip, INET_ADDRSTRLEN - 1);
    node->port = port;
    node->uid = uid;
    node->last_seen = time(NULL);
    node->online = true;
    
    g_node_count++;
    FF_LOG_INFO("Node added: %s:%u", ip, port);
    return FF_OK;
}

FFResult ff_p2p_node_remove(const uint8_t* node_id) {
    if (!node_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_node_count; ) {
        if (memcmp(g_nodes[i].node_id, node_id, 32) == 0) {
            memmove(&g_nodes[i], &g_nodes[i + 1], (g_node_count - i - 1) * sizeof(NodeInfo));
            g_node_count--;
            FF_LOG_INFO("Node removed");
            return FF_OK;
        } else {
            i++;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_p2p_node_get(const uint8_t* node_id, P2PNode* info) {
    if (!node_id || !info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_node_count; i++) {
        if (memcmp(g_nodes[i].node_id, node_id, 32) == 0) {
            memcpy(info->node_id, g_nodes[i].node_id, 32);
            strncpy(info->public_ip, g_nodes[i].ip, sizeof(info->public_ip) - 1);
            info->public_port = g_nodes[i].port;
            info->uid = g_nodes[i].uid;
            info->last_seen = g_nodes[i].last_seen;
            info->status = g_nodes[i].online ? 1 : 0;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_p2p_node_get_all(P2PNode** nodes, size_t* count) {
    if (!nodes || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_node_count == 0) {
        *nodes = NULL;
        *count = 0;
        return FF_OK;
    }
    
    *nodes = (P2PNode*)malloc(g_node_count * sizeof(P2PNode));
    if (!*nodes) {
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_node_count; i++) {
        P2PNode* node = &(*nodes)[i];
        memcpy(node->node_id, g_nodes[i].node_id, 32);
        strncpy(node->public_ip, g_nodes[i].ip, sizeof(node->public_ip) - 1);
        node->public_port = g_nodes[i].port;
        node->uid = g_nodes[i].uid;
        node->last_seen = g_nodes[i].last_seen;
        node->status = g_nodes[i].online ? 1 : 0;
    }

    *count = g_node_count;
    return FF_OK;
}

void ff_p2p_node_cleanup(void) {
    time_t now = time(NULL);
    for (size_t i = 0; i < g_node_count; ) {
        if (now - g_nodes[i].last_seen > 30) { // 30秒无响应视为离线
            g_nodes[i].online = false;
            i++;
        } else {
            i++;
        }
    }
}

bool ff_p2p_node_exists(const uint8_t* node_id) {
    if (!node_id) {
        return false;
    }
    
    for (size_t i = 0; i < g_node_count; i++) {
        if (memcmp(g_nodes[i].node_id, node_id, 32) == 0) {
            return true;
        }
    }
    return false;
}

size_t ff_p2p_node_get_count(void) {
    return g_node_count;
}

size_t ff_p2p_node_get_online_count(void) {
    size_t count = 0;
    for (size_t i = 0; i < g_node_count; i++) {
        if (g_nodes[i].online) {
            count++;
        }
    }
    return count;
}
