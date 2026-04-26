// FindFriend 用户管理模块

#include "ff_core_interface.h"
#include "ff_user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 1024

typedef struct {
    uint32_t uid;
    char username[64];
    char password[64];
    uint8_t node_id[32];
    bool online;
    uint64_t last_login;
} User;

static User g_users[MAX_USERS];
static size_t g_user_count = 0;
static uint32_t g_next_uid = 1000;

FFResult ff_user_init(void) {
    g_user_count = 0;
    g_next_uid = 1000;
    memset(g_users, 0, sizeof(g_users));
    FF_LOG_INFO("User manager initialized");
    return FF_OK;
}

void ff_user_cleanup(void) {
    g_user_count = 0;
    FF_LOG_INFO("User manager cleanup");
}

FFResult ff_user_register(const char* username, const char* password, const char* email, FFAsyncCallback callback, void* user_data) {
    if (!username || !password) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_user_count >= MAX_USERS) {
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (strcmp(g_users[i].username, username) == 0) {
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    User* user = &g_users[g_user_count];
    user->uid = g_next_uid++;
    strncpy(user->username, username, sizeof(user->username) - 1);
    strncpy(user->password, password, sizeof(user->password) - 1);
    user->online = false;
    user->last_login = 0;
    
    g_user_count++;
    
    FF_LOG_INFO("User registered: %s (UID: %u)", username, user->uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_user_login(const char* username, const char* password, FFAsyncCallback callback, void* user_data) {
    if (!username || !password) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (strcmp(g_users[i].username, username) == 0 &&
            strcmp(g_users[i].password, password) == 0) {
            g_users[i].online = true;
            g_users[i].last_login = time(NULL);
            FF_LOG_INFO("User logged in: %s (UID: %u)", username, g_users[i].uid);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    FF_LOG_ERROR("Login failed for user: %s", username);
    
    if (callback) {
        callback(FF_ERROR_PERMISSION, user_data);
    }
    
    return FF_ERROR_PERMISSION;
}

FFResult ff_user_logout(FFAsyncCallback callback, void* user_data) {
    // 简化实现，实际应该获取当前登录用户
    FF_LOG_INFO("User logged out");
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_user_auto_login(FFAsyncCallback callback, void* user_data) {
    // 简化实现，自动登录第一个用户
    if (g_user_count > 0) {
        FF_LOG_INFO("Auto login: %s (UID: %u)", g_users[0].username, g_users[0].uid);
        g_users[0].online = true;
        g_users[0].last_login = time(NULL);
        
        if (callback) {
            callback(FF_OK, user_data);
        }
        
        return FF_OK;
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_user_get_current(FFUser** user, FFAsyncCallback callback, void* user_data) {
    if (!user) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，返回第一个用户
    if (g_user_count > 0) {
        *user = (FFUser*)malloc(sizeof(FFUser));
        if (!*user) {
            if (callback) {
                callback(FF_ERROR_NO_MEMORY, user_data);
            }
            return FF_ERROR_NO_MEMORY;
        }
        
        memset(*user, 0, sizeof(FFUser));
        (*user)->uid = g_users[0].uid;
        strncpy((*user)->username, g_users[0].username, sizeof((*user)->username) - 1);
        (*user)->status = g_users[0].online ? 1 : 0;
        (*user)->last_active_time = g_users[0].last_login;
        
        if (callback) {
            callback(FF_OK, user_data);
        }
        
        return FF_OK;
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_user_get_by_id(uint32_t uid, FFUser** user, FFAsyncCallback callback, void* user_data) {
    if (!user) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid) {
            *user = (FFUser*)malloc(sizeof(FFUser));
            if (!*user) {
                if (callback) {
                    callback(FF_ERROR_NO_MEMORY, user_data);
                }
                return FF_ERROR_NO_MEMORY;
            }
            
            memset(*user, 0, sizeof(FFUser));
            (*user)->uid = g_users[i].uid;
            strncpy((*user)->username, g_users[i].username, sizeof((*user)->username) - 1);
            (*user)->status = g_users[i].online ? 1 : 0;
            (*user)->last_active_time = g_users[i].last_login;
            
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

FFResult ff_user_get_by_username(const char* username, FFUser** user, FFAsyncCallback callback, void* user_data) {
    if (!user || !username) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (strcmp(g_users[i].username, username) == 0) {
            *user = (FFUser*)malloc(sizeof(FFUser));
            if (!*user) {
                if (callback) {
                    callback(FF_ERROR_NO_MEMORY, user_data);
                }
                return FF_ERROR_NO_MEMORY;
            }
            
            memset(*user, 0, sizeof(FFUser));
            (*user)->uid = g_users[i].uid;
            strncpy((*user)->username, g_users[i].username, sizeof((*user)->username) - 1);
            (*user)->status = g_users[i].online ? 1 : 0;
            (*user)->last_active_time = g_users[i].last_login;
            
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

FFResult ff_user_update(FFUser* user, FFAsyncCallback callback, void* user_data) {
    if (!user) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == user->uid) {
            if (user->username[0]) {
                strncpy(g_users[i].username, user->username, sizeof(g_users[i].username) - 1);
            }
            g_users[i].online = (user->status == 1);
            FF_LOG_INFO("User info updated: UID %u", user->uid);
            
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

FFResult ff_user_update_password(uint32_t uid, const char* old_password, const char* new_password, FFAsyncCallback callback, void* user_data) {
    if (!old_password || !new_password) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid && strcmp(g_users[i].password, old_password) == 0) {
            strncpy(g_users[i].password, new_password, sizeof(g_users[i].password) - 1);
            FF_LOG_INFO("Password updated for user: UID %u", uid);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_PERMISSION, user_data);
    }
    
    return FF_ERROR_PERMISSION;
}

FFResult ff_user_set_status(uint8_t status) {
    // 简化实现，设置第一个用户的状态
    if (g_user_count > 0) {
        g_users[0].online = (status == 1);
        return FF_OK;
    }
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_user_get_status(uint32_t uid, uint8_t* status) {
    if (!status) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid) {
            *status = g_users[i].online ? 1 : 0;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_user_set_online(void) {
    // 简化实现，设置第一个用户为在线
    if (g_user_count > 0) {
        g_users[0].online = true;
        return FF_OK;
    }
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_user_set_offline(void) {
    // 简化实现，设置第一个用户为离线
    if (g_user_count > 0) {
        g_users[0].online = false;
        return FF_OK;
    }
    return FF_ERROR_NOT_FOUND;
}

void ff_user_free(FFUser* user) {
    if (user) {
        free(user);
    }
}

FFResult ff_friend_add(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    FF_LOG_INFO("Friend request sent to: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_remove(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    FF_LOG_INFO("Friend removed: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_accept(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    FF_LOG_INFO("Friend request accepted: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_reject(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    FF_LOG_INFO("Friend request rejected: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_block(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    FF_LOG_INFO("User blocked: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_unblock(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    FF_LOG_INFO("User unblocked: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_get_list(FFFriend** friends, size_t* count, FFAsyncCallback callback, void* user_data) {
    // 简化实现，返回空列表
    *friends = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_search(const char* keyword, FFFriend** results, size_t* count, FFAsyncCallback callback, void* user_data) {
    // 简化实现，返回空结果
    *results = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_is_friend(uint32_t uid, bool* is_friend) {
    if (!is_friend) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，始终返回 false
    *is_friend = false;
    return FF_OK;
}

FFResult ff_friend_get_relation(uint32_t uid, FFFriendRelation* relation) {
    if (!relation) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，始终返回无关系
    *relation = FF_FRIEND_NONE;
    return FF_OK;
}

void ff_friend_list_free(FFFriend* friends, size_t count) {
    if (friends) {
        free(friends);
    }
}

bool ff_user_exists(uint32_t uid) {
    for (size_t i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid) {
            return true;
        }
    }
    return false;
}

bool ff_user_is_online(uint32_t uid) {
    for (size_t i = 0; i < g_user_count; i++) {
        if (g_users[i].uid == uid) {
            return g_users[i].online;
        }
    }
    return false;
}
