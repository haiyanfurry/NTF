// FindFriend 消息管理模块

#include "ff_core_interface.h"
#include "ff_message_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGES 1024
#define MAX_MESSAGE_SIZE 4096

typedef struct {
    uint64_t mid;
    uint32_t sender_uid;
    uint32_t receiver_uid;
    char content[MAX_MESSAGE_SIZE];
    uint64_t timestamp;
    FFMessageType type;
    uint8_t status;
} Message;

static Message g_messages[MAX_MESSAGES];
static size_t g_message_count = 0;
static uint64_t g_next_mid = 1000000;

FFResult ff_message_init(void) {
    g_message_count = 0;
    g_next_mid = 1000000;
    memset(g_messages, 0, sizeof(g_messages));
    FF_LOG_INFO("Message manager initialized");
    return FF_OK;
}

void ff_message_cleanup(void) {
    g_message_count = 0;
    FF_LOG_INFO("Message manager cleanup");
}

FFResult ff_message_send(uint32_t to_uid, FFMessageType type, const char* content, FFMessage** out_msg, FFAsyncCallback callback, void* user_data) {
    if (!content) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_message_count >= MAX_MESSAGES) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    Message* msg = &g_messages[g_message_count];
    msg->mid = g_next_mid++;
    msg->sender_uid = 1000; // 临时值，实际应该从用户管理模块获取当前用户
    msg->receiver_uid = to_uid;
    strncpy(msg->content, content, sizeof(msg->content) - 1);
    msg->timestamp = time(NULL);
    msg->type = type;
    msg->status = 0; // 0: 待发送
    
    g_message_count++;
    
    if (out_msg) {
        *out_msg = (FFMessage*)malloc(sizeof(FFMessage));
        if (!*out_msg) {
            if (callback) {
                callback(FF_ERROR_NO_MEMORY, user_data);
            }
            return FF_ERROR_NO_MEMORY;
        }
        (*out_msg)->msg_id = msg->mid;
        (*out_msg)->from_uid = msg->sender_uid;
        (*out_msg)->to_uid = msg->receiver_uid;
        (*out_msg)->type = msg->type;
        (*out_msg)->status = FF_MSG_SENT;
        strncpy((*out_msg)->content, msg->content, sizeof((*out_msg)->content) - 1);
        (*out_msg)->timestamp = msg->timestamp;
        (*out_msg)->is_deleted = false;
    }
    
    FF_LOG_INFO("Message sent: MID %llu from %u to %u", msg->mid, msg->sender_uid, msg->receiver_uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_message_send_text(uint32_t to_uid, const char* text, FFMessage** out_msg, FFAsyncCallback callback, void* user_data) {
    return ff_message_send(to_uid, FF_MSG_TEXT, text, out_msg, callback, user_data);
}

FFResult ff_message_send_image(uint32_t to_uid, const char* image_path, FFMessage** out_msg, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    return ff_message_send(to_uid, FF_MSG_IMAGE, image_path, out_msg, callback, user_data);
}

FFResult ff_message_send_location(uint32_t to_uid, double lat, double lon, FFMessage** out_msg, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    char content[256];
    snprintf(content, sizeof(content), "%.6f,%.6f", lat, lon);
    return ff_message_send(to_uid, FF_MSG_LOCATION, content, out_msg, callback, user_data);
}

FFResult ff_message_receive(FFMessage** messages, size_t* count, FFAsyncCallback callback, void* user_data) {
    if (!messages || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    uint32_t uid = 1000; // 临时值，实际应该从用户管理模块获取当前用户
    size_t received_count = 0;
    for (size_t i = 0; i < g_message_count; i++) {
        if (g_messages[i].receiver_uid == uid) {
            received_count++;
        }
    }
    
    if (received_count == 0) {
        *messages = NULL;
        *count = 0;
        
        if (callback) {
            callback(FF_OK, user_data);
        }
        
        return FF_OK;
    }
    
    *messages = (FFMessage*)malloc(received_count * sizeof(FFMessage));
    if (!*messages) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    size_t index = 0;
    for (size_t i = 0; i < g_message_count; i++) {
        if (g_messages[i].receiver_uid == uid) {
            FFMessage* msg = &(*messages)[index];
            msg->msg_id = g_messages[i].mid;
            msg->from_uid = g_messages[i].sender_uid;
            msg->to_uid = g_messages[i].receiver_uid;
            msg->type = g_messages[i].type;
            msg->status = FF_MSG_DELIVERED;
            strncpy(msg->content, g_messages[i].content, sizeof(msg->content) - 1);
            msg->timestamp = g_messages[i].timestamp;
            msg->is_deleted = false;
            g_messages[i].status = 1; // 1: 已接收
            index++;
        }
    }
    
    *count = received_count;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_message_get(uint64_t msg_id, FFMessage** message, FFAsyncCallback callback, void* user_data) {
    if (!message) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_message_count; i++) {
        if (g_messages[i].mid == msg_id) {
            *message = (FFMessage*)malloc(sizeof(FFMessage));
            if (!*message) {
                if (callback) {
                    callback(FF_ERROR_NO_MEMORY, user_data);
                }
                return FF_ERROR_NO_MEMORY;
            }
            (*message)->msg_id = g_messages[i].mid;
            (*message)->from_uid = g_messages[i].sender_uid;
            (*message)->to_uid = g_messages[i].receiver_uid;
            (*message)->type = g_messages[i].type;
            (*message)->status = FF_MSG_READ;
            strncpy((*message)->content, g_messages[i].content, sizeof((*message)->content) - 1);
            (*message)->timestamp = g_messages[i].timestamp;
            (*message)->is_deleted = false;
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_message_mark_read(uint64_t msg_id, FFAsyncCallback callback, void* user_data) {
    for (size_t i = 0; i < g_message_count; i++) {
        if (g_messages[i].mid == msg_id) {
            g_messages[i].status = 2; // 2: 已读
            FF_LOG_INFO("Message marked as read: MID %llu", msg_id);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_message_mark_all_read(uint32_t from_uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    if (callback) {
        callback(FF_OK, user_data);
    }
    return FF_OK;
}

FFResult ff_message_delete(uint64_t msg_id, FFAsyncCallback callback, void* user_data) {
    for (size_t i = 0; i < g_message_count; ) {
        if (g_messages[i].mid == msg_id) {
            memmove(&g_messages[i], &g_messages[i + 1], (g_message_count - i - 1) * sizeof(Message));
            g_message_count--;
            FF_LOG_INFO("Message deleted: MID %llu", msg_id);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        } else {
            i++;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_conversation_get_list(FFConversation** conversations, size_t* count, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    *conversations = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_conversation_get_unread_count(uint32_t uid, uint32_t* count) {
    if (!count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *count = 0;
    for (size_t i = 0; i < g_message_count; i++) {
        if (g_messages[i].receiver_uid == uid && g_messages[i].status < 2) {
            (*count)++;
        }
    }
    
    return FF_OK;
}

FFResult ff_conversation_clear_unread(uint32_t uid) {
    // 简化实现
    return FF_OK;
}

FFResult ff_message_search(const char* keyword, FFMessage** messages, size_t* count, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    *messages = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

void ff_message_free(FFMessage* message) {
    if (message) {
        free(message);
    }
}

void ff_message_list_free(FFMessage* messages, size_t count) {
    if (messages) {
        free(messages);
    }
}

void ff_conversation_list_free(FFConversation* conversations, size_t count) {
    if (conversations) {
        free(conversations);
    }
}
