#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 社交回调函数类型
typedef void (*SocialCallback)(void* user_data, int status, const void* data);

// 好友结构体
typedef struct {
    char* user_id;
    char* username;
    char* nickname;
    char* avatar_hash;
    bool online;
    int64_t last_seen;
} Friend;

// 社交管理器结构体
typedef struct {
    void* network_manager;
    Friend* friends;
    size_t friend_count;
} SocialManager;

// 初始化社交管理器
SocialManager* social_manager_init();

// 销毁社交管理器
void social_manager_destroy(SocialManager* manager);

// 添加好友
bool social_manager_add_friend(SocialManager* manager, const char* target_user, SocialCallback callback, void* user_data);

// 删除好友
bool social_manager_remove_friend(SocialManager* manager, const char* target_user, SocialCallback callback, void* user_data);

// 获取好友列表
bool social_manager_get_friends(SocialManager* manager, SocialCallback callback, void* user_data);

// 搜索用户
bool social_manager_search_user(SocialManager* manager, const char* keyword, SocialCallback callback, void* user_data);

// 发送消息
bool social_manager_send_message(SocialManager* manager, const char* target_user, const char* message, SocialCallback callback, void* user_data);

// 获取消息列表
bool social_manager_get_messages(SocialManager* manager, const char* target_user, SocialCallback callback, void* user_data);

// 标记消息已读
bool social_manager_mark_message_read(SocialManager* manager, const char* message_id, SocialCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // SOCIAL_MANAGER_H