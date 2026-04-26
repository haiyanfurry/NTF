#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 消息结构体
typedef struct {
    char* message_id;
    char* sender_id;
    char* content;
    int64_t timestamp;
    bool is_sent;
    bool is_read;
} Message;

// 聊天窗口回调函数类型
typedef void (*MessageSendCallback)(void* user_data, const char* content);

// 聊天窗口结构体
typedef struct {
    void* native_window;
    char* friend_id;
    char* friend_name;
    Message** messages;
    size_t message_count;
    MessageSendCallback callback;
    void* user_data;
} ChatWindow;

// 创建聊天窗口
ChatWindow* chat_window_create(void* parent_window, const char* friend_id, const char* friend_name);

// 销毁聊天窗口
void chat_window_destroy(ChatWindow* window);

// 显示聊天窗口
void chat_window_show(ChatWindow* window);

// 隐藏聊天窗口
void chat_window_hide(ChatWindow* window);

// 设置消息列表
void chat_window_set_messages(ChatWindow* window, Message** messages, size_t count);

// 添加消息
void chat_window_add_message(ChatWindow* window, Message* message);

// 发送消息
void chat_window_send_message(ChatWindow* window, const char* content);

// 更新消息状态
void chat_window_update_message_status(ChatWindow* window, const char* message_id, bool is_sent, bool is_read);

// 设置消息发送回调
void chat_window_set_send_callback(ChatWindow* window, MessageSendCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // CHAT_WINDOW_H