#include "chat_window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建聊天窗口
ChatWindow* chat_window_create(void* parent_window, const char* friend_id, const char* friend_name) {
    ChatWindow* window = (ChatWindow*)malloc(sizeof(ChatWindow));
    if (!window) {
        fprintf(stderr, "Error allocating memory for chat window\n");
        return NULL;
    }
    
    // 初始化结构体
    window->native_window = NULL; // 由平台特定实现设置
    window->friend_id = strdup(friend_id);
    window->friend_name = strdup(friend_name);
    window->messages = NULL;
    window->message_count = 0;
    window->callback = NULL;
    window->user_data = NULL;
    
    return window;
}

// 销毁聊天窗口
void chat_window_destroy(ChatWindow* window) {
    if (window) {
        // 释放好友ID和名称
        if (window->friend_id) {
            free(window->friend_id);
        }
        if (window->friend_name) {
            free(window->friend_name);
        }
        
        // 释放消息列表
        if (window->messages) {
            for (size_t i = 0; i < window->message_count; i++) {
                if (window->messages[i]) {
                    free(window->messages[i]->message_id);
                    free(window->messages[i]->sender_id);
                    free(window->messages[i]->content);
                    free(window->messages[i]);
                }
            }
            free(window->messages);
        }
        
        // 平台特定实现应该释放 native_window
        free(window);
    }
}

// 显示聊天窗口
void chat_window_show(ChatWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing chat window with %s (%s)\n", window->friend_name, window->friend_id);
}

// 隐藏聊天窗口
void chat_window_hide(ChatWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Hiding chat window\n");
}

// 设置消息列表
void chat_window_set_messages(ChatWindow* window, Message** messages, size_t count) {
    if (!window) {
        return;
    }
    
    // 释放旧的消息列表
    if (window->messages) {
        for (size_t i = 0; i < window->message_count; i++) {
            if (window->messages[i]) {
                free(window->messages[i]->message_id);
                free(window->messages[i]->sender_id);
                free(window->messages[i]->content);
                free(window->messages[i]);
            }
        }
        free(window->messages);
    }
    
    // 设置新的消息列表
    window->messages = messages;
    window->message_count = count;
    
    // 由平台特定实现更新UI
    printf("Set message list with %zu messages\n", count);
}

// 添加消息
void chat_window_add_message(ChatWindow* window, Message* message) {
    if (!window || !message) {
        return;
    }
    
    // 重新分配内存
    Message** new_messages = (Message**)realloc(window->messages, (window->message_count + 1) * sizeof(Message*));
    if (!new_messages) {
        fprintf(stderr, "Error reallocating memory for message list\n");
        return;
    }
    
    // 添加新消息
    new_messages[window->message_count] = message;
    window->messages = new_messages;
    window->message_count++;
    
    // 由平台特定实现更新UI
    printf("Added message: %s\n", message->content);
}

// 发送消息
void chat_window_send_message(ChatWindow* window, const char* content) {
    if (!window || !content) {
        return;
    }
    
    // 调用回调函数
    if (window->callback) {
        window->callback(window->user_data, content);
    }
    
    // 由平台特定实现更新UI
    printf("Sent message: %s\n", content);
}

// 更新消息状态
void chat_window_update_message_status(ChatWindow* window, const char* message_id, bool is_sent, bool is_read) {
    if (!window || !message_id) {
        return;
    }
    
    // 查找消息
    for (size_t i = 0; i < window->message_count; i++) {
        if (strcmp(window->messages[i]->message_id, message_id) == 0) {
            window->messages[i]->is_sent = is_sent;
            window->messages[i]->is_read = is_read;
            // 由平台特定实现更新UI
            printf("Updated message %s status: sent=%d, read=%d\n", message_id, is_sent, is_read);
            break;
        }
    }
}

// 设置消息发送回调
void chat_window_set_send_callback(ChatWindow* window, MessageSendCallback callback, void* user_data) {
    if (!window) {
        return;
    }
    
    window->callback = callback;
    window->user_data = user_data;
}