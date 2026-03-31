#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include "User.h"

// 数据库操作结果
typedef enum {
    DB_SUCCESS = 0,
    DB_ERROR = 1,
    DB_USER_EXISTS = 2,
    DB_USER_NOT_FOUND = 3,
    DB_PASSWORD_ERROR = 4
} DBResult;

// 初始化数据库
int db_init(void);

// 关闭数据库
void db_close(void);

// 用户注册
DBResult db_register_user(const char *username, const char *password, UserIdentity identity);

// 用户登录
DBResult db_login_user(const char *username, const char *password, User *user);

// 检查用户名是否存在
int db_username_exists(const char *username);

// 获取用户信息
DBResult db_get_user(const char *username, User *user);

// 更新用户信息
DBResult db_update_user(const User *user);

#endif