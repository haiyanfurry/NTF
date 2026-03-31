#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <time.h>
#include <string.h>

// 用户身份标识
typedef enum {
    IDENTITY_NORMAL = 0,
    IDENTITY_COSPLAYER = 1,
    IDENTITY_BIANHE = 2,
    IDENTITY_OFFICIAL = 3,
    IDENTITY_EXHIBITOR = 4
} UserIdentity;

typedef struct {
    uint32_t id;
    char username[64];        // 唯一用户名（登录用）
    char nickname[64];        // 显示昵称
    char avatar_hash[32];
    char signature[256];
    UserIdentity identity;
    uint16_t tags;
    double latitude;
    double longitude;
    int location_enabled;
    time_t last_seen;
    int online;
    char exhibition_name[128];
    char exhibition_url[256];
    int followers;
    int following;
    char emoji[8];            // 默认 emoji
} User;

typedef struct {
    uint32_t count;
    User users[500];
} UserList;

void user_init(User *u);
void user_set_identity(User *u, UserIdentity identity);
const char* user_get_identity_icon(UserIdentity identity);
const char* user_get_identity_color(UserIdentity identity);

#endif
