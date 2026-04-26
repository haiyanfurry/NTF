#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>

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
int db_register_user(const char *username, const char *password, int identity);

// 用户登录
int db_login_user(const char *username, const char *password, void *user);

// 检查用户名是否存在
int db_username_exists(const char *username);

// 获取用户信息
int db_get_user(const char *username, void *user);

// 更新用户信息
int db_update_user(const void *user);

#endif