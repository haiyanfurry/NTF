// FindFriend P2P 好友管理实现

#include "ff_p2p_friend.h"
#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRIENDS 256
#define MAX_PENDING_REQUESTS 64

typedef struct {
    uint8_t uid[32];
    char nickname[128];
    uint64_t added_time;
    uint8_t status;
    P2PLocation last_location;
    bool location_shared;
} FriendInfo;

typedef struct {
    uint8_t uid[32];
    char message[256];
    uint64_t request_time;
    uint8_t status;
} FriendRequest;

static FriendInfo g_friends[MAX_FRIENDS];
static size_t g_friend_count = 0;
static FriendRequest g_pending_requests[MAX_PENDING_REQUESTS];
static size_t g_request_count = 0;

FFResult ff_friend_init(void) {
    g_friend_count = 0;
    g_request_count = 0;
    memset(g_friends, 0, sizeof(g_friends));
    memset(g_pending_requests, 0, sizeof(g_pending_requests));
    return FF_OK;
}

void ff_friend_cleanup(void) {
    g_friend_count = 0;
    g_request_count = 0;
}

FFResult ff_friend_add(const uint8_t* uid, const char* nickname) {
    if (!uid || g_friend_count >= MAX_FRIENDS) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_friend_count; i++) {
        if (memcmp(g_friends[i].uid, uid, 32) == 0) {
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    memcpy(g_friends[g_friend_count].uid, uid, 32);
    if (nickname) {
        strncpy(g_friends[g_friend_count].nickname, nickname, sizeof(g_friends[g_friend_count].nickname) - 1);
    }
    g_friends[g_friend_count].added_time = time(NULL);
    g_friends[g_friend_count].status = 1;
    g_friends[g_friend_count].location_shared = false;
    
    g_friend_count++;
    
    return FF_OK;
}

FFResult ff_friend_remove(const uint8_t* uid) {
    if (!uid) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_friend_count; i++) {
        if (memcmp(g_friends[i].uid, uid, 32) == 0) {
            memmove(&g_friends[i], &g_friends[i + 1],
                   (g_friend_count - i - 1) * sizeof(FriendInfo));
            g_friend_count--;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_friend_get(const uint8_t* uid, FriendInfo** friend_info) {
    if (!uid || !friend_info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_friend_count; i++) {
        if (memcmp(g_friends[i].uid, uid, 32) == 0) {
            *friend_info = &g_friends[i];
            return FF_OK;
        }
    }
    
    *friend_info = NULL;
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_friend_get_all(FriendInfo** friends, size_t* count) {
    if (!friends || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *friends = g_friends;
    *count = g_friend_count;
    
    return FF_OK;
}

FFResult ff_friend_update_location(const uint8_t* uid, const P2PLocation* location) {
    if (!uid || !location) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FriendInfo* friend_info;
    FFResult result = ff_friend_get(uid, &friend_info);
    if (result != FF_OK) {
        return result;
    }
    
    friend_info->last_location = *location;
    friend_info->location_shared = true;
    
    return FF_OK;
}

FFResult ff_friend_get_location(const uint8_t* uid, P2PLocation* location, bool* shared) {
    if (!uid || !location) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FriendInfo* friend_info;
    FFResult result = ff_friend_get(uid, &friend_info);
    if (result != FF_OK) {
        return result;
    }
    
    if (!friend_info->location_shared) {
        return FF_ERROR;
    }
    
    *location = friend_info->last_location;
    if (shared) {
        *shared = friend_info->location_shared;
    }
    
    return FF_OK;
}

FFResult ff_friend_set_status(const uint8_t* uid, uint8_t status) {
    if (!uid) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FriendInfo* friend_info;
    FFResult result = ff_friend_get(uid, &friend_info);
    if (result != FF_OK) {
        return result;
    }
    
    friend_info->status = status;
    
    return FF_OK;
}

FFResult ff_friend_send_request(const uint8_t* target_uid, const char* message) {
    if (!target_uid || g_request_count >= MAX_PENDING_REQUESTS) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_request_count; i++) {
        if (memcmp(g_pending_requests[i].uid, target_uid, 32) == 0) {
            return FF_ERROR_ALREADY_INITIALIZED;
        }
    }
    
    memcpy(g_pending_requests[g_request_count].uid, target_uid, 32);
    if (message) {
        strncpy(g_pending_requests[g_request_count].message, message,
               sizeof(g_pending_requests[g_request_count].message) - 1);
    }
    g_pending_requests[g_request_count].request_time = time(NULL);
    g_pending_requests[g_request_count].status = 0;
    
    g_request_count++;
    
    return FF_OK;
}

FFResult ff_friend_accept_request(const uint8_t* requester_uid) {
    if (!requester_uid) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_request_count; i++) {
        if (memcmp(g_pending_requests[i].uid, requester_uid, 32) == 0) {
            FFResult result = ff_friend_add(requester_uid, NULL);
            if (result == FF_OK) {
                memmove(&g_pending_requests[i], &g_pending_requests[i + 1],
                       (g_request_count - i - 1) * sizeof(FriendRequest));
                g_request_count--;
            }
            return result;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_friend_reject_request(const uint8_t* requester_uid) {
    if (!requester_uid) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_request_count; i++) {
        if (memcmp(g_pending_requests[i].uid, requester_uid, 32) == 0) {
            memmove(&g_pending_requests[i], &g_pending_requests[i + 1],
                   (g_request_count - i - 1) * sizeof(FriendRequest));
            g_request_count--;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_friend_get_pending_requests(FriendRequest** requests, size_t* count) {
    if (!requests || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *requests = g_pending_requests;
    *count = g_request_count;
    
    return FF_OK;
}

FFResult ff_friend_is_friend(const uint8_t* uid, bool* is_friend) {
    if (!uid || !is_friend) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_friend_count; i++) {
        if (memcmp(g_friends[i].uid, uid, 32) == 0) {
            *is_friend = true;
            return FF_OK;
        }
    }
    
    *is_friend = false;
    return FF_OK;
}

size_t ff_friend_get_count(void) {
    return g_friend_count;
}
