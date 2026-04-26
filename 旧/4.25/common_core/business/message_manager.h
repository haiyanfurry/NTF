#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "../model/message.h"

// 消息管理器
typedef struct {
    void* network_manager;      // 网络管理器
    Message* messages;          // 消息列表
    size_t message_count;        // 消息数量
    DoNotDisturb dnd_settings;   // 免打扰设置
    bool push_enabled;           // 是否启用推送
} MessageManager;

// 消息回调函数类型
typedef void (*MessageCallback)(void* user_data, int status, Message* messages);

// 初始化消息管理器
MessageManager* message_manager_init();

// 销毁消息管理器
void message_manager_destroy(MessageManager* manager);

// 发送消息
bool message_manager_send_message(MessageManager* manager, uint32_t to_id, uint32_t group_id, MessageType type, const char* content, const char* media_path, MessageCallback callback, void* user_data);

// 接收消息
bool message_manager_receive_message(MessageManager* manager, Message* message);

// 获取消息列表
bool message_manager_get_messages(MessageManager* manager, uint32_t user_id, uint32_t group_id, int count, int offset, MessageCallback callback, void* user_data);

// 标记消息已读
bool message_manager_mark_message_read(MessageManager* manager, uint32_t message_id, MessageCallback callback, void* user_data);

// 标记所有消息已读
bool message_manager_mark_all_read(MessageManager* manager, uint32_t user_id, uint32_t group_id, MessageCallback callback, void* user_data);

// 删除消息
bool message_manager_delete_message(MessageManager* manager, uint32_t message_id, MessageCallback callback, void* user_data);

// 设置免打扰
bool message_manager_set_do_not_disturb(MessageManager* manager, int global_dnd, int official_only, MessageCallback callback, void* user_data);

// 静音群组
bool message_manager_mute_group(MessageManager* manager, uint32_t group_id, bool mute, MessageCallback callback, void* user_data);

// 检查是否免打扰
bool message_manager_is_do_not_disturb(MessageManager* manager, uint32_t sender_id, uint32_t group_id, int is_official);

// 启用/禁用推送
bool message_manager_set_push_enabled(MessageManager* manager, bool enabled);

// 获取未读消息数
size_t message_manager_get_unread_count(MessageManager* manager, uint32_t user_id, uint32_t group_id);

// 清理消息
bool message_manager_cleanup_messages(MessageManager* manager, uint32_t user_id, uint32_t group_id, time_t before_time);

#endif // MESSAGE_MANAGER_H
