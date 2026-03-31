#ifndef FINDFRIENDAPP_DATABASE_H
#define FINDFRIENDAPP_DATABASE_H

#include <string>
#include "User.h"

class Database {
private:
    std::string dbPath;

public:
    Database();
    ~Database();

    // 初始化数据库
    bool init();

    // 注册用户
    bool registerUser(const User& user);

    // 登录验证
    bool login(const std::string& username, const std::string& password, User& user);

    // 检查用户名是否存在
    bool isUsernameExists(const std::string& username);

    // 获取用户信息
    bool getUserInfo(const std::string& username, User& user);

    // 更新用户信息
    bool updateUserInfo(const User& user);

    // 删除用户
    bool deleteUser(const std::string& username);

private:
    // 执行SQL语句
    bool executeSQL(const std::string& sql);

    // 获取数据库路径
    std::string getDatabasePath();
};

#endif // FINDFRIENDAPP_DATABASE_H