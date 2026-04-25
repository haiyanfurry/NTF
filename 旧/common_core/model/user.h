#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

struct User {
    uint32_t id;
    char* username;
    char* nickname;
    char* avatar;
    char* signature;
    int identity;      // 0:normal,1:cos,2:bianhe,3:official,4:exhibitor
    uint16_t tags;     // bit flags: 1 furry, 2 anime, 4 cos
    double latitude;
    double longitude;
    bool locationEnabled;
    time_t lastSeen;
    bool online;
    char* exhibitionName;
    char* exhibitionUrl;
    int followers;
    int following;
    char* emoji;
};

// 初始化用户结构体
void user_init(struct User* user);

// 释放用户结构体资源
void user_free(struct User* user);

// 验证用户数据
bool user_validate(const struct User* user);

#endif