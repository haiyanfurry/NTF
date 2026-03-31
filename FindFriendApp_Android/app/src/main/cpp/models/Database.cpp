#include "Database.h"
#include <sqlite3.h>
#include <android/log.h>
#include <string>

#define TAG "Database"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

Database::Database() {
    dbPath = getDatabasePath();
}

Database::~Database() {
}

bool Database::init() {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    // 创建用户表
    std::string sql = "CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "username TEXT UNIQUE,"
                     "password TEXT,"
                     "user_id TEXT,"
                     "has_badge INTEGER,"
                     "is_official INTEGER"
                     ");";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    LOGI("Database initialized successfully");
    return true;
}

bool Database::registerUser(const User& user) {
    if (!user.validate()) {
        LOGE("Invalid user data");
        return false;
    }

    if (isUsernameExists(user.getUsername())) {
        LOGE("Username already exists");
        return false;
    }

    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    std::string sql = "INSERT INTO users (username, password, user_id, has_badge, is_official) "
                     "VALUES ('" + user.getUsername() + "', '" + user.getPassword() + "', '" + user.getUserId() + "', "
                     + std::to_string(user.getHasBadge()) + ", " + std::to_string(user.getIsOfficial()) + ");";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    LOGI("User registered successfully: %s", user.getUsername().c_str());
    return true;
}

bool Database::login(const std::string& username, const std::string& password, User& user) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    std::string sql = "SELECT username, password, user_id, has_badge, is_official FROM users WHERE username = '" + username + "' AND password = '" + password + "';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user.setUsername(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        user.setPassword(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        user.setUserId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        user.setHasBadge(sqlite3_column_int(stmt, 3) == 1);
        user.setIsOfficial(sqlite3_column_int(stmt, 4) == 1);
        found = true;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (found) {
        LOGI("Login successful: %s", username.c_str());
    } else {
        LOGE("Login failed: %s", username.c_str());
    }

    return found;
}

bool Database::isUsernameExists(const std::string& username) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    std::string sql = "SELECT COUNT(*) FROM users WHERE username = '" + username + "';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return count > 0;
}

bool Database::getUserInfo(const std::string& username, User& user) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    std::string sql = "SELECT username, password, user_id, has_badge, is_official FROM users WHERE username = '" + username + "';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user.setUsername(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        user.setPassword(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        user.setUserId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        user.setHasBadge(sqlite3_column_int(stmt, 3) == 1);
        user.setIsOfficial(sqlite3_column_int(stmt, 4) == 1);
        found = true;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return found;
}

bool Database::updateUserInfo(const User& user) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    std::string sql = "UPDATE users SET password = '" + user.getPassword() + "', user_id = '" + user.getUserId() + "', "
                     "has_badge = " + std::to_string(user.getHasBadge()) + ", is_official = " + std::to_string(user.getIsOfficial()) + " "
                     "WHERE username = '" + user.getUsername() + "';";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    LOGI("User info updated successfully: %s", user.getUsername().c_str());
    return true;
}

bool Database::deleteUser(const std::string& username) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    std::string sql = "DELETE FROM users WHERE username = '" + username + "';";
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    LOGI("User deleted successfully: %s", username.c_str());
    return true;
}

bool Database::executeSQL(const std::string& sql) {
    sqlite3* db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return false;
    }

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

std::string Database::getDatabasePath() {
    // 在Android平台，数据库文件通常存储在应用的私有目录中
    // 这里返回一个默认路径，实际应用中应该使用Android的API获取正确路径
    return "/data/data/com.findfriend.app/databases/findfriend.db";
}