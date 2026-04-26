#ifndef FRIEND_LIST_H
#define FRIEND_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 好友结构体
typedef struct {
    char* user_id;
    char* username;
    char* nickname;
    char* avatar_hash;
    bool online;
    int64_t last_seen;
} UIFriend;

// 好友列表回调函数类型
typedef void (*FriendSelectCallback)(void* user_data, const char* friend_id);

// 好友列表结构体
typedef struct {
    void* native_list;
    UIFriend** friends;
    size_t friend_count;
    FriendSelectCallback callback;
    void* user_data;
} FriendList;

// 创建好友列表
FriendList* friend_list_create(void* parent_window);

// 销毁好友列表
void friend_list_destroy(FriendList* list);

// 显示好友列表
void friend_list_show(FriendList* list);

// 隐藏好友列表
void friend_list_hide(FriendList* list);

// 设置好友列表
void friend_list_set_friends(FriendList* list, UIFriend** friends, size_t count);

// 添加好友
void friend_list_add_friend(FriendList* list, UIFriend* friend);

// 移除好友
void friend_list_remove_friend(FriendList* list, const char* friend_id);

// 更新好友状态
void friend_list_update_friend_status(FriendList* list, const char* friend_id, bool online);

// 设置好友选择回调
void friend_list_set_select_callback(FriendList* list, FriendSelectCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // FRIEND_LIST_H