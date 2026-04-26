// FindFriend P2P 好友管理接口

#ifndef FF_P2P_FRIEND_H
#define FF_P2P_FRIEND_H

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"
#include <stdbool.h>

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

FFResult ff_friend_init(void);

void ff_friend_cleanup(void);

FFResult ff_friend_add(const uint8_t* uid, const char* nickname);

FFResult ff_friend_remove(const uint8_t* uid);

FFResult ff_friend_get(const uint8_t* uid, FriendInfo** friend_info);

FFResult ff_friend_get_all(FriendInfo** friends, size_t* count);

FFResult ff_friend_update_location(const uint8_t* uid, const P2PLocation* location);

FFResult ff_friend_get_location(const uint8_t* uid, P2PLocation* location, bool* shared);

FFResult ff_friend_set_status(const uint8_t* uid, uint8_t status);

FFResult ff_friend_send_request(const uint8_t* target_uid, const char* message);

FFResult ff_friend_accept_request(const uint8_t* requester_uid);

FFResult ff_friend_reject_request(const uint8_t* requester_uid);

FFResult ff_friend_get_pending_requests(FriendRequest** requests, size_t* count);

FFResult ff_friend_is_friend(const uint8_t* uid, bool* is_friend);

size_t ff_friend_get_count(void);

#endif // FF_P2P_FRIEND_H
