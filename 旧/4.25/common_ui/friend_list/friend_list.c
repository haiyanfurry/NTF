#include "friend_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建好友列表
FriendList* friend_list_create(void* parent_window) {
    FriendList* list = (FriendList*)malloc(sizeof(FriendList));
    if (!list) {
        fprintf(stderr, "Error allocating memory for friend list\n");
        return NULL;
    }
    
    // 初始化结构体
    list->native_list = NULL; // 由平台特定实现设置
    list->friends = NULL;
    list->friend_count = 0;
    list->callback = NULL;
    list->user_data = NULL;
    
    return list;
}

// 销毁好友列表
void friend_list_destroy(FriendList* list) {
    if (list) {
        // 释放好友列表
        if (list->friends) {
            for (size_t i = 0; i < list->friend_count; i++) {
                if (list->friends[i]) {
                    free(list->friends[i]->user_id);
                    free(list->friends[i]->username);
                    free(list->friends[i]->nickname);
                    free(list->friends[i]->avatar_hash);
                    free(list->friends[i]);
                }
            }
            free(list->friends);
        }
        
        // 平台特定实现应该释放 native_list
        free(list);
    }
}

// 显示好友列表
void friend_list_show(FriendList* list) {
    if (!list) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing friend list with %zu friends\n", list->friend_count);
}

// 隐藏好友列表
void friend_list_hide(FriendList* list) {
    if (!list) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Hiding friend list\n");
}

// 设置好友列表
void friend_list_set_friends(FriendList* list, UIFriend** friends, size_t count) {
    if (!list) {
        return;
    }
    
    // 释放旧的好友列表
    if (list->friends) {
        for (size_t i = 0; i < list->friend_count; i++) {
            if (list->friends[i]) {
                free(list->friends[i]->user_id);
                free(list->friends[i]->username);
                free(list->friends[i]->nickname);
                free(list->friends[i]->avatar_hash);
                free(list->friends[i]);
            }
        }
        free(list->friends);
    }
    
    // 设置新的好友列表
    list->friends = friends;
    list->friend_count = count;
    
    // 由平台特定实现更新UI
    printf("Set friend list with %zu friends\n", count);
}

// 添加好友
void friend_list_add_friend(FriendList* list, UIFriend* friend) {
    if (!list || !friend) {
        return;
    }
    
    // 重新分配内存
    UIFriend** new_friends = (UIFriend**)realloc(list->friends, (list->friend_count + 1) * sizeof(UIFriend*));
    if (!new_friends) {
        fprintf(stderr, "Error reallocating memory for friend list\n");
        return;
    }
    
    // 添加新好友
    new_friends[list->friend_count] = friend;
    list->friends = new_friends;
    list->friend_count++;
    
    // 由平台特定实现更新UI
    printf("Added friend: %s\n", friend->username);
}

// 移除好友
void friend_list_remove_friend(FriendList* list, const char* friend_id) {
    if (!list || !friend_id) {
        return;
    }
    
    // 查找好友
    size_t index = -1;
    for (size_t i = 0; i < list->friend_count; i++) {
        if (strcmp(list->friends[i]->user_id, friend_id) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return;
    }
    
    // 释放好友内存
    free(list->friends[index]->user_id);
    free(list->friends[index]->username);
    free(list->friends[index]->nickname);
    free(list->friends[index]->avatar_hash);
    free(list->friends[index]);
    
    // 移动其他好友
    for (size_t i = index; i < list->friend_count - 1; i++) {
        list->friends[i] = list->friends[i + 1];
    }
    
    // 重新分配内存
    UIFriend** new_friends = (UIFriend**)realloc(list->friends, (list->friend_count - 1) * sizeof(UIFriend*));
    if (new_friends || list->friend_count == 1) {
        list->friends = new_friends;
        list->friend_count--;
    }
    
    // 由平台特定实现更新UI
    printf("Removed friend: %s\n", friend_id);
}

// 更新好友状态
void friend_list_update_friend_status(FriendList* list, const char* friend_id, bool online) {
    if (!list || !friend_id) {
        return;
    }
    
    // 查找好友
    for (size_t i = 0; i < list->friend_count; i++) {
        if (strcmp(list->friends[i]->user_id, friend_id) == 0) {
            list->friends[i]->online = online;
            // 由平台特定实现更新UI
            printf("Updated friend %s status to %s\n", friend_id, online ? "online" : "offline");
            break;
        }
    }
}

// 设置好友选择回调
void friend_list_set_select_callback(FriendList* list, FriendSelectCallback callback, void* user_data) {
    if (!list) {
        return;
    }
    
    list->callback = callback;
    list->user_data = user_data;
}