#include "User.h"
#include <cstdlib>
#include <ctime>
#include <android/log.h>

#define TAG "User"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

User::User() : hasBadge(false), isOfficial(false) {
    generateUserId();
}

User::User(const std::string& username, const std::string& password) 
    : username(username), password(password), hasBadge(false), isOfficial(false) {
    generateUserId();
}

User::~User() {
}

std::string User::getUsername() const {
    return username;
}

std::string User::getPassword() const {
    return password;
}

std::string User::getUserId() const {
    return userId;
}

bool User::getHasBadge() const {
    return hasBadge;
}

bool User::getIsOfficial() const {
    return isOfficial;
}

void User::setUsername(const std::string& username) {
    this->username = username;
}

void User::setPassword(const std::string& password) {
    this->password = password;
}

void User::setUserId(const std::string& userId) {
    this->userId = userId;
}

void User::setHasBadge(bool hasBadge) {
    this->hasBadge = hasBadge;
}

void User::setIsOfficial(bool isOfficial) {
    this->isOfficial = isOfficial;
}

bool User::validate() const {
    if (username.empty() || password.empty()) {
        LOGE("Username or password is empty");
        return false;
    }
    if (username.length() < 3 || username.length() > 20) {
        LOGE("Username length must be between 3 and 20 characters");
        return false;
    }
    if (password.length() < 6) {
        LOGE("Password length must be at least 6 characters");
        return false;
    }
    return true;
}

void User::generateUserId() {
    // 生成随机用户ID
    srand(time(nullptr));
    char id[37]; // 36字符 + 结束符
    const char* chars = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 36; i++) {
        id[i] = chars[rand() % 36];
    }
    id[36] = '\0';
    userId = std::string(id);
    LOGI("Generated user ID: %s", userId.c_str());
}