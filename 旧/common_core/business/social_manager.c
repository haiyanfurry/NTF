// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "social_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 社交响应回调函数
static void social_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    SocialCallback callback = (SocialCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    SocialManager* manager = (SocialManager*)((void**)user_data)[2];
    const char* action = (const char*)((void**)user_data)[3];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 根据操作类型处理响应
        if (strcmp(action, "add_friend") == 0) {
            printf("Friend added successfully\n");
        } else if (strcmp(action, "remove_friend") == 0) {
            printf("Friend removed successfully\n");
        } else if (strcmp(action, "get_friends") == 0) {
            // 模拟好友列表
            // 实际应用中应该解析服务器返回的好友列表
            if (manager->friends) {
                free(manager->friends);
            }
            manager->friend_count = 2;
            manager->friends = (Friend*)malloc(sizeof(Friend) * manager->friend_count);
            if (manager->friends) {
                // 好友 1
                manager->friends[0].user_id = strdup("user_123");
                manager->friends[0].username = strdup("friend1");
                manager->friends[0].nickname = strdup("好友1");
                manager->friends[0].avatar_hash = strdup("avatar1");
                manager->friends[0].online = true;
                manager->friends[0].last_seen = time(NULL);
                
                // 好友 2
                manager->friends[1].user_id = strdup("user_456");
                manager->friends[1].username = strdup("friend2");
                manager->friends[1].nickname = strdup("好友2");
                manager->friends[1].avatar_hash = strdup("avatar2");
                manager->friends[1].online = false;
                manager->friends[1].last_seen = time(NULL) - 3600;
            }
        } else if (strcmp(action, "search_user") == 0) {
            printf("User searched successfully\n");
        } else if (strcmp(action, "send_message") == 0) {
            printf("Message sent successfully\n");
        } else if (strcmp(action, "get_messages") == 0) {
            printf("Messages retrieved successfully\n");
        } else if (strcmp(action, "mark_read") == 0) {
            printf("Message marked as read\n");
        }
        
        // 调用回调函数
        if (callback) {
            callback(callback_user_data, status, manager->friends);
        }
    } else {
        // 操作失败
        if (callback) {
            callback(callback_user_data, status, NULL);
        }
    }
    
    // 清理资源
    free((void*)action);
    free(user_data);
}

// 初始化社交管理器
SocialManager* social_manager_init() {
    SocialManager* manager = (SocialManager*)malloc(sizeof(SocialManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->friends = NULL;
    manager->friend_count = 0;
    
    return manager;
}

// 销毁社交管理器
void social_manager_destroy(SocialManager* manager) {
    if (manager) {
        // 释放好友列表
        if (manager->friends) {
            for (size_t i = 0; i < manager->friend_count; i++) {
                if (manager->friends[i].user_id) free(manager->friends[i].user_id);
                if (manager->friends[i].username) free(manager->friends[i].username);
                if (manager->friends[i].nickname) free(manager->friends[i].nickname);
                if (manager->friends[i].avatar_hash) free(manager->friends[i].avatar_hash);
            }
            free(manager->friends);
        }
        free(manager);
    }
}

// 添加好友
bool social_manager_add_friend(SocialManager* manager, const char* target_user, SocialCallback callback, void* user_data) {
    if (!manager || !target_user) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("add_friend");
    
    // 模拟添加好友请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Adding friend: %s\n", target_user);
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 删除好友
bool social_manager_remove_friend(SocialManager* manager, const char* target_user, SocialCallback callback, void* user_data) {
    if (!manager || !target_user) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("remove_friend");
    
    // 模拟删除好友请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Removing friend: %s\n", target_user);
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 获取好友列表
bool social_manager_get_friends(SocialManager* manager, SocialCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("get_friends");
    
    // 模拟获取好友列表请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting friends list\n");
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 搜索用户
bool social_manager_search_user(SocialManager* manager, const char* keyword, SocialCallback callback, void* user_data) {
    if (!manager || !keyword) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("search_user");
    
    // 模拟搜索用户请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Searching users with keyword: %s\n", keyword);
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 发送消息
bool social_manager_send_message(SocialManager* manager, const char* target_user, const char* message, SocialCallback callback, void* user_data) {
    if (!manager || !target_user || !message) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("send_message");
    
    // 构建消息数据
    char* data = (char*)malloc(1024);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 1024, "{\"target_user\": \"%s\", \"message\": \"%s\"}", target_user, message);
    
    // 模拟发送消息请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Sending message to %s: %s\n", target_user, message);
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 获取消息列表
bool social_manager_get_messages(SocialManager* manager, const char* target_user, SocialCallback callback, void* user_data) {
    if (!manager || !target_user) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("get_messages");
    
    // 构建消息请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"target_user\": \"%s\"}", target_user);
    
    // 模拟获取消息列表请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting messages from %s\n", target_user);
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 标记消息已读
bool social_manager_mark_message_read(SocialManager* manager, const char* message_id, SocialCallback callback, void* user_data) {
    if (!manager || !message_id) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("mark_read");
    
    // 构建标记已读请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"message_id\": \"%s\"}", message_id);
    
    // 模拟标记已读请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Marking message as read: %s\n", message_id);
    
    // 模拟请求成功
    social_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}