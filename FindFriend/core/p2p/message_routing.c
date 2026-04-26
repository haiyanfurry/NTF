// FindFriend P2P 消息路由模块

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define MAX_ROUTING_TABLE_SIZE 1024
#define MAX_MESSAGE_QUEUE_SIZE 1024

// 路由表条目
typedef struct {
    uint32_t uid;
    uint8_t node_id[32];
    uint32_t conn_id;
    uint64_t last_update;
} RoutingEntry;

// 消息队列条目
typedef struct {
    uint32_t sender_uid;
    uint32_t receiver_uid;
    uint8_t* data;
    size_t length;
    uint64_t timestamp;
} MessageQueueEntry;

static RoutingEntry g_routing_table[MAX_ROUTING_TABLE_SIZE];
static size_t g_routing_table_size = 0;
static MessageQueueEntry g_message_queue[MAX_MESSAGE_QUEUE_SIZE];
static size_t g_message_queue_size = 0;
static pthread_mutex_t g_routing_lock;
static pthread_mutex_t g_queue_lock;

// 函数声明
extern FFResult ff_p2p_connection_send(uint32_t conn_id, const uint8_t* data, size_t length);

FFResult ff_p2p_message_routing_init(void) {
    g_routing_table_size = 0;
    g_message_queue_size = 0;
    memset(g_routing_table, 0, sizeof(g_routing_table));
    memset(g_message_queue, 0, sizeof(g_message_queue));
    pthread_mutex_init(&g_routing_lock, NULL);
    pthread_mutex_init(&g_queue_lock, NULL);
    FF_LOG_INFO("Message routing initialized");
    return FF_OK;
}

void ff_p2p_message_routing_cleanup(void) {
    // 清理消息队列
    for (size_t i = 0; i < g_message_queue_size; i++) {
        if (g_message_queue[i].data) {
            free(g_message_queue[i].data);
        }
    }
    g_routing_table_size = 0;
    g_message_queue_size = 0;
    FF_LOG_INFO("Message routing cleanup");
}

FFResult ff_p2p_message_routing_add_route(uint32_t uid, const uint8_t* node_id, uint32_t conn_id) {
    if (!node_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    pthread_mutex_lock(&g_routing_lock);
    
    // 查找现有路由
    for (size_t i = 0; i < g_routing_table_size; i++) {
        if (g_routing_table[i].uid == uid) {
            // 更新路由
            memcpy(g_routing_table[i].node_id, node_id, 32);
            g_routing_table[i].conn_id = conn_id;
            g_routing_table[i].last_update = time(NULL);
            pthread_mutex_unlock(&g_routing_lock);
            return FF_OK;
        }
    }
    
    // 添加新路由
    if (g_routing_table_size >= MAX_ROUTING_TABLE_SIZE) {
        pthread_mutex_unlock(&g_routing_lock);
        return FF_ERROR_NO_MEMORY;
    }
    
    RoutingEntry* entry = &g_routing_table[g_routing_table_size];
    entry->uid = uid;
    memcpy(entry->node_id, node_id, 32);
    entry->conn_id = conn_id;
    entry->last_update = time(NULL);
    
    g_routing_table_size++;
    pthread_mutex_unlock(&g_routing_lock);
    FF_LOG_INFO("Route added: UID %u -> conn %u", uid, conn_id);
    return FF_OK;
}

FFResult ff_p2p_message_routing_remove_route(uint32_t uid) {
    pthread_mutex_lock(&g_routing_lock);
    
    for (size_t i = 0; i < g_routing_table_size; ) {
        if (g_routing_table[i].uid == uid) {
            memmove(&g_routing_table[i], &g_routing_table[i + 1], 
                    (g_routing_table_size - i - 1) * sizeof(RoutingEntry));
            g_routing_table_size--;
            pthread_mutex_unlock(&g_routing_lock);
            FF_LOG_INFO("Route removed: UID %u", uid);
            return FF_OK;
        } else {
            i++;
        }
    }
    
    pthread_mutex_unlock(&g_routing_lock);
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_p2p_message_routing_find_route(uint32_t uid, uint32_t* conn_id) {
    if (!conn_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    pthread_mutex_lock(&g_routing_lock);
    
    for (size_t i = 0; i < g_routing_table_size; i++) {
        if (g_routing_table[i].uid == uid) {
            *conn_id = g_routing_table[i].conn_id;
            pthread_mutex_unlock(&g_routing_lock);
            return FF_OK;
        }
    }
    
    pthread_mutex_unlock(&g_routing_lock);
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_p2p_message_routing_send(uint32_t sender_uid, uint32_t receiver_uid, const uint8_t* data, size_t length) {
    if (!data) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 查找路由
    uint32_t conn_id = 0;
    FFResult res = ff_p2p_message_routing_find_route(receiver_uid, &conn_id);
    if (res != FF_OK) {
        // 添加到消息队列
        pthread_mutex_lock(&g_queue_lock);
        
        if (g_message_queue_size >= MAX_MESSAGE_QUEUE_SIZE) {
            pthread_mutex_unlock(&g_queue_lock);
            return FF_ERROR_NO_MEMORY;
        }
        
        MessageQueueEntry* entry = &g_message_queue[g_message_queue_size];
        entry->sender_uid = sender_uid;
        entry->receiver_uid = receiver_uid;
        entry->data = (uint8_t*)malloc(length);
        if (!entry->data) {
            pthread_mutex_unlock(&g_queue_lock);
            return FF_ERROR_NO_MEMORY;
        }
        memcpy(entry->data, data, length);
        entry->length = length;
        entry->timestamp = time(NULL);
        
        g_message_queue_size++;
        pthread_mutex_unlock(&g_queue_lock);
        FF_LOG_INFO("Message queued for UID %u", receiver_uid);
        return FF_OK;
    }
    
    // 直接发送消息
    res = ff_p2p_connection_send(conn_id, data, length);
    if (res == FF_OK) {
        FF_LOG_INFO("Message sent from %u to %u", sender_uid, receiver_uid);
    }
    
    return res;
}

FFResult ff_p2p_message_routing_process_queue(void) {
    pthread_mutex_lock(&g_queue_lock);
    
    size_t processed = 0;
    for (size_t i = 0; i < g_message_queue_size; ) {
        MessageQueueEntry* entry = &g_message_queue[i];
        
        // 查找路由
        uint32_t conn_id = 0;
        FFResult res = ff_p2p_message_routing_find_route(entry->receiver_uid, &conn_id);
        if (res == FF_OK) {
            // 发送消息
            res = ff_p2p_connection_send(conn_id, entry->data, entry->length);
            if (res == FF_OK) {
                // 消息发送成功，从队列中移除
                free(entry->data);
                memmove(&g_message_queue[i], &g_message_queue[i + 1], 
                        (g_message_queue_size - i - 1) * sizeof(MessageQueueEntry));
                g_message_queue_size--;
                processed++;
                FF_LOG_INFO("Queued message sent from %u to %u", entry->sender_uid, entry->receiver_uid);
                continue;
            }
        }
        
        // 消息超过5分钟未发送，移除
        if (time(NULL) - entry->timestamp > 300) {
            free(entry->data);
            memmove(&g_message_queue[i], &g_message_queue[i + 1], 
                    (g_message_queue_size - i - 1) * sizeof(MessageQueueEntry));
            g_message_queue_size--;
            FF_LOG_INFO("Queued message expired for UID %u", entry->receiver_uid);
            continue;
        }
        
        i++;
    }
    
    pthread_mutex_unlock(&g_queue_lock);
    
    if (processed > 0) {
        FF_LOG_INFO("Processed %zu queued messages", processed);
    }
    
    return FF_OK;
}

FFResult ff_p2p_message_routing_cleanup_routes(void) {
    time_t now = time(NULL);
    pthread_mutex_lock(&g_routing_lock);
    
    for (size_t i = 0; i < g_routing_table_size; ) {
        if (now - g_routing_table[i].last_update > 300) {
            // 5分钟无更新，移除路由
            memmove(&g_routing_table[i], &g_routing_table[i + 1], 
                    (g_routing_table_size - i - 1) * sizeof(RoutingEntry));
            g_routing_table_size--;
            FF_LOG_INFO("Route expired: UID %u", g_routing_table[i].uid);
            continue;
        }
        i++;
    }
    
    pthread_mutex_unlock(&g_routing_lock);
    return FF_OK;
}

FFResult ff_p2p_message_routing_get_routes(uint32_t** uids, size_t* count) {
    if (!uids || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    pthread_mutex_lock(&g_routing_lock);
    
    if (g_routing_table_size == 0) {
        *uids = NULL;
        *count = 0;
        pthread_mutex_unlock(&g_routing_lock);
        return FF_OK;
    }
    
    *uids = (uint32_t*)malloc(g_routing_table_size * sizeof(uint32_t));
    if (!*uids) {
        pthread_mutex_unlock(&g_routing_lock);
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_routing_table_size; i++) {
        (*uids)[i] = g_routing_table[i].uid;
    }
    
    *count = g_routing_table_size;
    pthread_mutex_unlock(&g_routing_lock);
    return FF_OK;
}

FFResult ff_p2p_message_routing_free_routes(uint32_t* uids) {
    if (uids) {
        free(uids);
    }
    return FF_OK;
}

FFResult ff_p2p_message_routing_get_queue_size(size_t* size) {
    if (!size) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    pthread_mutex_lock(&g_queue_lock);
    *size = g_message_queue_size;
    pthread_mutex_unlock(&g_queue_lock);
    return FF_OK;
}

size_t ff_p2p_message_routing_get_route_count(void) {
    pthread_mutex_lock(&g_routing_lock);
    size_t count = g_routing_table_size;
    pthread_mutex_unlock(&g_routing_lock);
    return count;
}
