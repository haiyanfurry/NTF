// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化用户结构体
void user_init(struct User* user) {
    if (user) {
        user->id = 0;
        user->username = NULL;
        user->nickname = NULL;
        user->avatar = NULL;
        user->signature = NULL;
        user->identity = 0;
        user->tags = 1;
        user->latitude = 0.0;
        user->longitude = 0.0;
        user->locationEnabled = true;
        user->lastSeen = time(NULL);
        user->online = false;
        user->exhibitionName = NULL;
        user->exhibitionUrl = NULL;
        user->followers = 0;
        user->following = 0;
        user->emoji = strdup("🐾");
    }
}

// 释放用户结构体资源
void user_free(struct User* user) {
    if (user) {
        if (user->username) {
            free(user->username);
            user->username = NULL;
        }
        if (user->nickname) {
            free(user->nickname);
            user->nickname = NULL;
        }
        if (user->avatar) {
            free(user->avatar);
            user->avatar = NULL;
        }
        if (user->signature) {
            free(user->signature);
            user->signature = NULL;
        }
        if (user->exhibitionName) {
            free(user->exhibitionName);
            user->exhibitionName = NULL;
        }
        if (user->exhibitionUrl) {
            free(user->exhibitionUrl);
            user->exhibitionUrl = NULL;
        }
        if (user->emoji) {
            free(user->emoji);
            user->emoji = NULL;
        }
    }
}

// 验证用户数据
bool user_validate(const struct User* user) {
    if (!user) {
        return false;
    }
    
    // 验证用户名
    if (!user->username || strlen(user->username) == 0) {
        return false;
    }
    
    // 验证身份
    if (user->identity < 0 || user->identity > 4) {
        return false;
    }
    
    // 验证标签
    if (user->tags > 7) { // 只有低3位有效
        return false;
    }
    
    return true;
}