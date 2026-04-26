// ====================================================================
// FindFriend 用户管理接口
// 业务模块：用户注册、登录、信息管理
// ====================================================================

#ifndef FF_USER_INTERFACE_H
#define FF_USER_INTERFACE_H

#include "ff_core_interface.h"
#include <stdint.h>

// ====================================================================
// 用户数据结构
// ====================================================================
typedef struct FFUser {
    uint32_t uid;
    char username[64];
    char email[128];
    char phone[32];
    char nickname[64];
    char avatar_url[256];
    uint8_t status;  // 0=offline, 1=online, 2=away
    double latitude;
    double longitude;
    uint64_t last_active_time;
    uint32_t friend_count;
    bool is_verified;
} FFUser;

// ====================================================================
// 好友关系
// ====================================================================
typedef enum FFFriendRelation {
    FF_FRIEND_NONE = 0,
    FF_FRIEND_PENDING = 1,
    FF_FRIEND_CONFIRMED = 2,
    FF_FRIEND_BLOCKED = 3
} FFFriendRelation;

typedef struct FFFriend {
    uint32_t uid;
    char username[64];
    char nickname[64];
    FFFriendRelation relation;
    double latitude;
    double longitude;
    uint64_t last_active_time;
    uint8_t status;
} FFFriend;

// ====================================================================
// 用户管理接口
// ====================================================================

// 用户注册与登录
FFResult ff_user_register(const char* username, const char* password, const char* email, FFAsyncCallback callback, void* user_data);
FFResult ff_user_login(const char* username, const char* password, FFAsyncCallback callback, void* user_data);
FFResult ff_user_logout(FFAsyncCallback callback, void* user_data);
FFResult ff_user_auto_login(FFAsyncCallback callback, void* user_data);

// 用户信息获取
FFResult ff_user_get_current(FFUser** user, FFAsyncCallback callback, void* user_data);
FFResult ff_user_get_by_id(uint32_t uid, FFUser** user, FFAsyncCallback callback, void* user_data);
FFResult ff_user_get_by_username(const char* username, FFUser** user, FFAsyncCallback callback, void* user_data);
FFResult ff_user_update(FFUser* user, FFAsyncCallback callback, void* user_data);
FFResult ff_user_update_password(uint32_t uid, const char* old_password, const char* new_password, FFAsyncCallback callback, void* user_data);

// 用户状态
FFResult ff_user_set_status(uint8_t status);
FFResult ff_user_get_status(uint32_t uid, uint8_t* status);
FFResult ff_user_set_online(void);
FFResult ff_user_set_offline(void);

// 好友管理
FFResult ff_friend_add(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_remove(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_accept(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_reject(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_block(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_unblock(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_get_list(FFFriend** friends, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_friend_search(const char* keyword, FFFriend** results, size_t* count, FFAsyncCallback callback, void* user_data);

// 好友关系查询
FFResult ff_friend_is_friend(uint32_t uid, bool* is_friend);
FFResult ff_friend_get_relation(uint32_t uid, FFFriendRelation* relation);

// 内存释放
void ff_user_free(FFUser* user);
void ff_friend_list_free(FFFriend* friends, size_t count);

#endif // FF_USER_INTERFACE_H
