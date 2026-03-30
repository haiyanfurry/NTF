#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <time.h>

typedef struct {
    uint32_t id;
    char nickname[64];
    char emoji[8];
    char signature[128];
    uint8_t avatar[32];
    double latitude;
    double longitude;
    uint16_t tags;  // 1:furry 2:anime 4:cos
    uint8_t online;
    time_t last_seen;
} User;

typedef struct {
    uint32_t count;
    User users[100];
} UserList;

// 初始化用户
void user_init(User *u, uint32_t id, const char *name, const char *emoji, const char *sig);

#endif
