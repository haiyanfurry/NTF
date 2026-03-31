#include "Database.h"
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static sqlite3 *db = NULL;

// 哈希密码（简单实现，实际项目中应使用更安全的哈希算法）
static void hash_password(const char *password, char *hash) {
    unsigned int hash_val = 5381;
    int c;
    while ((c = *password++)) {
        hash_val = ((hash_val << 5) + hash_val) + c;
    }
    sprintf(hash, "%u", hash_val);
}

// 初始化数据库
int db_init(void) {
    int rc = sqlite3_open("findfriend.db", &db);
    if (rc != SQLITE_OK) {
        return 0;
    }
    
    // 创建用户表
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "username TEXT UNIQUE,"
                     "password TEXT,"
                     "nickname TEXT,"
                     "avatar_hash TEXT,"
                     "signature TEXT,"
                     "identity INTEGER,"
                     "tags INTEGER,"
                     "latitude REAL,"
                     "longitude REAL,"
                     "location_enabled INTEGER,"
                     "last_seen INTEGER,"
                     "online INTEGER,"
                     "exhibition_name TEXT,"
                     "exhibition_url TEXT,"
                     "followers INTEGER,"
                     "following INTEGER,"
                     "emoji TEXT"
                     ");";
    
    char *err_msg = NULL;
    rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        return 0;
    }
    
    return 1;
}

// 关闭数据库
void db_close(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

// 用户注册
DBResult db_register_user(const char *username, const char *password, UserIdentity identity) {
    // 检查用户名是否已存在
    if (db_username_exists(username)) {
        return DB_USER_EXISTS;
    }
    
    char password_hash[32];
    hash_password(password, password_hash);
    
    char sql[512];
    sprintf(sql, "INSERT INTO users (username, password, nickname, identity) VALUES ('%s', '%s', '%s', %d);",
            username, password_hash, username, identity);
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        return DB_ERROR;
    }
    
    return DB_SUCCESS;
}

// 用户登录
DBResult db_login_user(const char *username, const char *password, User *user) {
    char password_hash[32];
    hash_password(password, password_hash);
    
    char sql[512];
    sprintf(sql, "SELECT * FROM users WHERE username = '%s' AND password = '%s';",
            username, password_hash);
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return DB_ERROR;
    }
    
    int result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return DB_USER_NOT_FOUND;
    }
    
    // 填充用户信息
    user->id = sqlite3_column_int(stmt, 0);
    strncpy(user->username, (const char*)sqlite3_column_text(stmt, 1), 63);
    strncpy(user->nickname, (const char*)sqlite3_column_text(stmt, 3), 63);
    strncpy(user->avatar_hash, (const char*)sqlite3_column_text(stmt, 4), 31);
    strncpy(user->signature, (const char*)sqlite3_column_text(stmt, 5), 255);
    user->identity = sqlite3_column_int(stmt, 6);
    user->tags = sqlite3_column_int(stmt, 7);
    user->latitude = sqlite3_column_double(stmt, 8);
    user->longitude = sqlite3_column_double(stmt, 9);
    user->location_enabled = sqlite3_column_int(stmt, 10);
    user->last_seen = sqlite3_column_int(stmt, 11);
    user->online = 1;
    strncpy(user->exhibition_name, (const char*)sqlite3_column_text(stmt, 13), 127);
    strncpy(user->exhibition_url, (const char*)sqlite3_column_text(stmt, 14), 255);
    user->followers = sqlite3_column_int(stmt, 15);
    user->following = sqlite3_column_int(stmt, 16);
    strncpy(user->emoji, (const char*)sqlite3_column_text(stmt, 17), 7);
    
    // 更新在线状态和最后登录时间
    time_t now = time(NULL);
    sprintf(sql, "UPDATE users SET online = 1, last_seen = %ld WHERE id = %d;", now, user->id);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    
    sqlite3_finalize(stmt);
    return DB_SUCCESS;
}

// 检查用户名是否存在
int db_username_exists(const char *username) {
    char sql[256];
    sprintf(sql, "SELECT 1 FROM users WHERE username = '%s';", username);
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return 0;
    }
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return (result == SQLITE_ROW);
}

// 获取用户信息
DBResult db_get_user(const char *username, User *user) {
    char sql[256];
    sprintf(sql, "SELECT * FROM users WHERE username = '%s';", username);
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        return DB_ERROR;
    }
    
    int result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return DB_USER_NOT_FOUND;
    }
    
    // 填充用户信息
    user->id = sqlite3_column_int(stmt, 0);
    strncpy(user->username, (const char*)sqlite3_column_text(stmt, 1), 63);
    strncpy(user->nickname, (const char*)sqlite3_column_text(stmt, 3), 63);
    strncpy(user->avatar_hash, (const char*)sqlite3_column_text(stmt, 4), 31);
    strncpy(user->signature, (const char*)sqlite3_column_text(stmt, 5), 255);
    user->identity = sqlite3_column_int(stmt, 6);
    user->tags = sqlite3_column_int(stmt, 7);
    user->latitude = sqlite3_column_double(stmt, 8);
    user->longitude = sqlite3_column_double(stmt, 9);
    user->location_enabled = sqlite3_column_int(stmt, 10);
    user->last_seen = sqlite3_column_int(stmt, 11);
    user->online = sqlite3_column_int(stmt, 12);
    strncpy(user->exhibition_name, (const char*)sqlite3_column_text(stmt, 13), 127);
    strncpy(user->exhibition_url, (const char*)sqlite3_column_text(stmt, 14), 255);
    user->followers = sqlite3_column_int(stmt, 15);
    user->following = sqlite3_column_int(stmt, 16);
    strncpy(user->emoji, (const char*)sqlite3_column_text(stmt, 17), 7);
    
    sqlite3_finalize(stmt);
    return DB_SUCCESS;
}

// 更新用户信息
DBResult db_update_user(const User *user) {
    char sql[512];
    sprintf(sql, "UPDATE users SET nickname = '%s', avatar_hash = '%s', signature = '%s', "
            "identity = %d, tags = %d, latitude = %f, longitude = %f, location_enabled = %d, "
            "last_seen = %ld, online = %d, exhibition_name = '%s', exhibition_url = '%s', "
            "followers = %d, following = %d, emoji = '%s' WHERE id = %d;",
            user->nickname, user->avatar_hash, user->signature,
            user->identity, user->tags, user->latitude, user->longitude, user->location_enabled,
            user->last_seen, user->online, user->exhibition_name, user->exhibition_url,
            user->followers, user->following, user->emoji, user->id);
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        return DB_ERROR;
    }
    
    return DB_SUCCESS;
}