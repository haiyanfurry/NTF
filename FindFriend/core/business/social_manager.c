// FindFriend 社交管理模块

#include "ff_core_interface.h"
#include "ff_user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FRIENDS 1024
#define MAX_BLOCKED 1024

typedef struct {
    uint32_t user_uid;
    uint32_t friend_uid;
    uint64_t since;
    uint8_t status; // 0: 待确认, 1: 已确认
} Friend;

typedef struct {
    uint32_t user_uid;
    uint32_t blocked_uid;
    uint64_t since;
} BlockedUser;

static Friend g_friends[MAX_FRIENDS];
static size_t g_friend_count = 0;
static BlockedUser g_blocked[MAX_BLOCKED];
static size_t g_blocked_count = 0;

FFResult ff_social_init(void) {
    g_friend_count = 0;
    g_blocked_count = 0;
    memset(g_friends, 0, sizeof(g_friends));
    memset(g_blocked, 0, sizeof(g_blocked));
    FF_LOG_INFO("Social manager initialized");
    return FF_OK;
}

void ff_social_cleanup(void) {
    g_friend_count = 0;
    g_blocked_count = 0;
    FF_LOG_INFO("Social manager cleanup");
}

FFResult ff_friend_add(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    // 简化实现，假设当前用户为发送者
    uint32_t sender_uid = 1000; // 临时值
    
    if (sender_uid == uid) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_friend_count >= MAX_FRIENDS) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_friend_count; i++) {
        if ((g_friends[i].user_uid == sender_uid && g_friends[i].friend_uid == uid) ||
            (g_friends[i].user_uid == uid && g_friends[i].friend_uid == sender_uid)) {
            if (callback) {
                callback(FF_ERROR_ALREADY_INITIALIZED, user_data);
            }
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    Friend* friend = &g_friends[g_friend_count];
    friend->user_uid = sender_uid;
    friend->friend_uid = uid;
    friend->since = time(NULL);
    friend->status = 0;
    
    g_friend_count++;
    FF_LOG_INFO("Friend request sent: %u -> %u", sender_uid, uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_remove(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    uint32_t user_uid = 1000; // 临时值
    
    for (size_t i = 0; i < g_friend_count; ) {
        if ((g_friends[i].user_uid == user_uid && g_friends[i].friend_uid == uid) ||
            (g_friends[i].user_uid == uid && g_friends[i].friend_uid == user_uid)) {
            memmove(&g_friends[i], &g_friends[i + 1], (g_friend_count - i - 1) * sizeof(Friend));
            g_friend_count--;
        } else {
            i++;
        }
    }
    
    FF_LOG_INFO("Friend removed: %u <-> %u", user_uid, uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_accept(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    uint32_t user_uid = 1000; // 临时值
    
    for (size_t i = 0; i < g_friend_count; i++) {
        if (g_friends[i].user_uid == uid && g_friends[i].friend_uid == user_uid) {
            g_friends[i].status = 1;
            
            if (g_friend_count >= MAX_FRIENDS) {
                if (callback) {
                    callback(FF_ERROR_NO_MEMORY, user_data);
                }
                return FF_ERROR_NO_MEMORY;
            }
            
            Friend* friend = &g_friends[g_friend_count];
            friend->user_uid = user_uid;
            friend->friend_uid = uid;
            friend->since = time(NULL);
            friend->status = 1;
            
            g_friend_count++;
            FF_LOG_INFO("Friend request accepted: %u <-> %u", user_uid, uid);
            
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

FFResult ff_friend_reject(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    uint32_t user_uid = 1000; // 临时值
    
    for (size_t i = 0; i < g_friend_count; ) {
        if (g_friends[i].user_uid == uid && g_friends[i].friend_uid == user_uid) {
            memmove(&g_friends[i], &g_friends[i + 1], (g_friend_count - i - 1) * sizeof(Friend));
            g_friend_count--;
            FF_LOG_INFO("Friend request rejected: %u -> %u", uid, user_uid);
            
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

FFResult ff_friend_block(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    uint32_t user_uid = 1000; // 临时值
    
    if (user_uid == uid) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_blocked_count >= MAX_BLOCKED) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_blocked_count; i++) {
        if (g_blocked[i].user_uid == user_uid && g_blocked[i].blocked_uid == uid) {
            if (callback) {
                callback(FF_ERROR_ALREADY_INITIALIZED, user_data);
            }
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    BlockedUser* blocked = &g_blocked[g_blocked_count];
    blocked->user_uid = user_uid;
    blocked->blocked_uid = uid;
    blocked->since = time(NULL);
    
    g_blocked_count++;
    FF_LOG_INFO("User blocked: %u -> %u", user_uid, uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_unblock(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    uint32_t user_uid = 1000; // 临时值
    
    for (size_t i = 0; i < g_blocked_count; i++) {
        if (g_blocked[i].user_uid == user_uid && g_blocked[i].blocked_uid == uid) {
            memmove(&g_blocked[i], &g_blocked[i + 1], (g_blocked_count - i - 1) * sizeof(BlockedUser));
            g_blocked_count--;
            FF_LOG_INFO("User unblocked: %u -> %u", user_uid, uid);
            
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

FFResult ff_friend_get_list(FFFriend** friends, size_t* count, FFAsyncCallback callback, void* user_data) {
    uint32_t uid = 1000; // 临时值
    
    if (!friends || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t friend_count = 0;
    for (size_t i = 0; i < g_friend_count; i++) {
        if (g_friends[i].user_uid == uid && g_friends[i].status == 1) {
            friend_count++;
        }
    }
    
    if (friend_count == 0) {
        *friends = NULL;
        *count = 0;
        
        if (callback) {
            callback(FF_OK, user_data);
        }
        
        return FF_OK;
    }
    
    *friends = (FFFriend*)malloc(friend_count * sizeof(FFFriend));
    if (!*friends) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    size_t index = 0;
    for (size_t i = 0; i < g_friend_count; i++) {
        if (g_friends[i].user_uid == uid && g_friends[i].status == 1) {
            FFFriend* friend = &(*friends)[index];
            friend->uid = g_friends[i].friend_uid;
            // 简化实现，实际应该从用户管理模块获取用户名等信息
            snprintf(friend->username, sizeof(friend->username), "user_%u", g_friends[i].friend_uid);
            friend->relation = FF_FRIEND_CONFIRMED;
            friend->status = 1;
            index++;
        }
    }
    
    *count = friend_count;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_search(const char* keyword, FFFriend** results, size_t* count, FFAsyncCallback callback, void* user_data) {
    // 简化实现
    *results = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_friend_is_friend(uint32_t uid, bool* is_friend) {
    uint32_t user_uid = 1000; // 临时值
    
    if (!is_friend) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *is_friend = false;
    for (size_t i = 0; i < g_friend_count; i++) {
        if ((g_friends[i].user_uid == user_uid && g_friends[i].friend_uid == uid && g_friends[i].status == 1) ||
            (g_friends[i].user_uid == uid && g_friends[i].friend_uid == user_uid && g_friends[i].status == 1)) {
            *is_friend = true;
            break;
        }
    }
    
    return FF_OK;
}

FFResult ff_friend_get_relation(uint32_t uid, FFFriendRelation* relation) {
    uint32_t user_uid = 1000; // 临时值
    
    if (!relation) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *relation = FF_FRIEND_NONE;
    
    // 检查是否是好友
    for (size_t i = 0; i < g_friend_count; i++) {
        if ((g_friends[i].user_uid == user_uid && g_friends[i].friend_uid == uid) ||
            (g_friends[i].user_uid == uid && g_friends[i].friend_uid == user_uid)) {
            *relation = g_friends[i].status == 1 ? FF_FRIEND_CONFIRMED : FF_FRIEND_PENDING;
            return FF_OK;
        }
    }
    
    // 检查是否被阻止
    for (size_t i = 0; i < g_blocked_count; i++) {
        if (g_blocked[i].user_uid == user_uid && g_blocked[i].blocked_uid == uid) {
            *relation = FF_FRIEND_BLOCKED;
            return FF_OK;
        }
    }
    
    return FF_OK;
}

void ff_friend_list_free(FFFriend* friends, size_t count) {
    if (friends) {
        free(friends);
    }
}
