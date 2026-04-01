#include "database.h"
#include "user.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
char* getDatabasePath();

#define TAG "Database"
#define LOGD(...) printf("[DEBUG] " __VA_ARGS__)
#define LOGI(...) printf("[INFO] " __VA_ARGS__)
#define LOGE(...) printf("[ERROR] " __VA_ARGS__)

static sqlite3* db = NULL;
static char* dbPath = NULL;

// 初始化数据库
int db_init(void) {
    // 获取数据库路径
    dbPath = getDatabasePath();
    if (!dbPath) {
        LOGE("Failed to get database path\n");
        return DB_ERROR;
    }
    
    // 打开数据库
    int rc = sqlite3_open(dbPath, &db);
    if (rc) {
        LOGE("Can't open database: %s\n", sqlite3_errmsg(db));
        free(dbPath);
        return DB_ERROR;
    }

    // 创建用户表
    char* sql = "CREATE TABLE IF NOT EXISTS users ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "username TEXT UNIQUE,"
                 "password TEXT,"
                 "user_id TEXT,"
                 "has_badge INTEGER,"
                 "is_official INTEGER"
                 ");";

    char* errMsg = NULL;
    rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        free(dbPath);
        return DB_ERROR;
    }

    LOGI("Database initialized successfully\n");
    return DB_SUCCESS;
}

// 关闭数据库
void db_close(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
    if (dbPath) {
        free(dbPath);
        dbPath = NULL;
    }
}

// 用户注册
int db_register_user(const char *username, const char *password, int identity) {
    if (!db || !username || !password) {
        return DB_ERROR;
    }

    // 检查用户名是否存在
    if (db_username_exists(username)) {
        return DB_USER_EXISTS;
    }

    // 构建SQL语句
    char sql[512];
    sprintf(sql, "INSERT INTO users (username, password, user_id, has_badge, is_official) "
            "VALUES ('%s', '%s', '', %d, %d);",
            username, password, (identity == 1) ? 1 : 0, (identity == 3) ? 1 : 0);

    char* errMsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DB_ERROR;
    }

    LOGI("User registered successfully: %s\n", username);
    return DB_SUCCESS;
}

// 用户登录
int db_login_user(const char *username, const char *password, void *user) {
    if (!db || !username || !password || !user) {
        return DB_ERROR;
    }

    // 构建SQL语句
    char sql[512];
    sprintf(sql, "SELECT username, password, user_id, has_badge, is_official FROM users WHERE username = '%s' AND password = '%s';",
            username, password);

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s\n", sqlite3_errmsg(db));
        return DB_ERROR;
    }

    int result = DB_USER_NOT_FOUND;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        struct User* user_ptr = (struct User*)user;
        
        // 设置用户信息
        const char* db_username = (const char*)sqlite3_column_text(stmt, 0);
        const char* db_password = (const char*)sqlite3_column_text(stmt, 1);
        const char* db_user_id = (const char*)sqlite3_column_text(stmt, 2);
        int has_badge = sqlite3_column_int(stmt, 3);
        int is_official = sqlite3_column_int(stmt, 4);
        
        user_ptr->username = strdup(db_username);
        user_ptr->identity = 0;
        if (has_badge) {
            user_ptr->identity = 1;
        } else if (is_official) {
            user_ptr->identity = 3;
        }
        
        result = DB_SUCCESS;
    }

    sqlite3_finalize(stmt);
    return result;
}

// 检查用户名是否存在
int db_username_exists(const char *username) {
    if (!db || !username) {
        return 0;
    }

    // 构建SQL语句
    char sql[512];
    sprintf(sql, "SELECT COUNT(*) FROM users WHERE username = '%s';", username);

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

// 获取用户信息
int db_get_user(const char *username, void *user) {
    if (!db || !username || !user) {
        return DB_ERROR;
    }

    // 构建SQL语句
    char sql[512];
    sprintf(sql, "SELECT username, password, user_id, has_badge, is_official FROM users WHERE username = '%s';", username);

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s\n", sqlite3_errmsg(db));
        return DB_ERROR;
    }

    int result = DB_USER_NOT_FOUND;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        struct User* user_ptr = (struct User*)user;
        
        // 设置用户信息
        const char* db_username = (const char*)sqlite3_column_text(stmt, 0);
        const char* db_password = (const char*)sqlite3_column_text(stmt, 1);
        const char* db_user_id = (const char*)sqlite3_column_text(stmt, 2);
        int has_badge = sqlite3_column_int(stmt, 3);
        int is_official = sqlite3_column_int(stmt, 4);
        
        user_ptr->username = strdup(db_username);
        user_ptr->identity = 0;
        if (has_badge) {
            user_ptr->identity = 1;
        } else if (is_official) {
            user_ptr->identity = 3;
        }
        
        result = DB_SUCCESS;
    }

    sqlite3_finalize(stmt);
    return result;
}

// 更新用户信息
int db_update_user(const void *user) {
    if (!db || !user) {
        return DB_ERROR;
    }

    const struct User* user_ptr = (const struct User*)user;
    if (!user_ptr->username) {
        return DB_ERROR;
    }

    // 构建SQL语句
    char sql[512];
    sprintf(sql, "UPDATE users SET password = '', user_id = '', "
            "has_badge = %d, is_official = %d "
            "WHERE username = '%s';",
            (user_ptr->identity == 1) ? 1 : 0, (user_ptr->identity == 3) ? 1 : 0,
            user_ptr->username);

    char* errMsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return DB_ERROR;
    }

    LOGI("User info updated successfully: %s\n", user_ptr->username);
    return DB_SUCCESS;
}

// 获取数据库路径
char* getDatabasePath() {
    // 返回默认路径，实际应用中应该根据平台获取正确路径
    return strdup("findfriend.db");
}