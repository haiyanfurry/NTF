// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "user_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 函数声明
static void free_user_resources(User* user);
static User* copy_user_data(const User* source);

/**
 * @brief 登录响应回调函数
 * @param user_data 回调数据，包含回调函数、用户数据和用户管理器
 * @param status 响应状态码
 * @param data 响应数据
 * @param length 响应数据长度
 * @return 无
 */
static void login_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    LoginCallback callback = (LoginCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    UserManager* manager = (UserManager*)((void**)user_data)[2];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 创建用户对象
        User temp_user;
        temp_user.user_id = "user_123456";
        temp_user.username = "test_user";
        temp_user.nickname = "测试用户";
        temp_user.avatar_hash = "avatar_hash_123";
        temp_user.signature = "这是一个测试签名";
        temp_user.identity = USER_IDENTITY_NORMAL;
        temp_user.tags = 0;
        temp_user.latitude = 39.9042;
        temp_user.longitude = 116.4074;
        temp_user.location_enabled = true;
        temp_user.last_seen = time(NULL);
        temp_user.online = true;
        temp_user.exhibition_name = "测试展会";
        temp_user.exhibition_url = "http://example.com/exhibition";
        temp_user.followers = 10;
        temp_user.following = 5;
        temp_user.emoji = "😊";
        
        // 复制用户数据
        User* user = copy_user_data(&temp_user);
        if (user) {
            // 更新当前用户
            if (manager->current_user) {
                free_user_resources(manager->current_user);
            }
            manager->current_user = user;
            manager->logged_in = true;
            
            // 调用回调函数
            if (callback) {
                callback(callback_user_data, status, user);
            }
        } else {
            // 内存分配失败
            if (callback) {
                callback(callback_user_data, 500, NULL);
            }
        }
    } else {
        // 登录失败
        if (callback) {
            callback(callback_user_data, status, NULL);
        }
    }
    
    // 清理资源
    free(user_data);
}



/**
 * @brief 释放用户资源
 * @param user 用户对象指针
 * @return 无
 */
static void free_user_resources(User* user) {
    if (user) {
        if (user->user_id) free(user->user_id);
        if (user->username) free(user->username);
        if (user->nickname) free(user->nickname);
        if (user->avatar_hash) free(user->avatar_hash);
        if (user->signature) free(user->signature);
        if (user->exhibition_name) free(user->exhibition_name);
        if (user->exhibition_url) free(user->exhibition_url);
        if (user->emoji) free(user->emoji);
        free(user);
    }
}

/**
 * @brief 初始化用户管理器
 * @return 用户管理器指针，失败返回 NULL
 */
UserManager* user_manager_init() {
    UserManager* manager = (UserManager*)malloc(sizeof(UserManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->current_user = NULL;
    manager->logged_in = false;
    
    return manager;
}

/**
 * @brief 销毁用户管理器
 * @param manager 用户管理器指针
 * @return 无
 */
void user_manager_destroy(UserManager* manager) {
    if (manager) {
        user_manager_logout(manager);
        free(manager);
    }
}

/**
 * @brief 用户登录
 * @param manager 用户管理器指针
 * @param username 用户名
 * @param password 密码
 * @param device_fingerprint 设备指纹
 * @param callback 登录回调函数
 * @param user_data 用户数据
 * @return 登录请求是否发送成功
 */
bool user_manager_login(UserManager* manager, const char* username, const char* password, const char* device_fingerprint, LoginCallback callback, void* user_data) {
    if (!manager || !username || !password || !device_fingerprint) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 3);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    
    // 模拟登录请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Login request sent for user: %s\n", username);
    
    // 模拟登录成功
    login_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

/**
 * @brief 用户登出
 * @param manager 用户管理器指针
 * @return 无
 */
void user_manager_logout(UserManager* manager) {
    if (manager) {
        // 释放当前用户资源
        if (manager->current_user) {
            free_user_resources(manager->current_user);
            manager->current_user = NULL;
        }
        
        manager->logged_in = false;
    }
}

/**
 * @brief 获取当前用户
 * @param manager 用户管理器指针
 * @return 当前用户指针，未登录返回 NULL
 */
User* user_manager_get_current_user(UserManager* manager) {
    if (!manager) {
        return NULL;
    }
    return manager->current_user;
}

/**
 * @brief 复制用户数据
 * @param source 源用户对象指针
 * @return 复制后的用户对象指针，失败返回 NULL
 */
static User* copy_user_data(const User* source) {
    if (!source) {
        return NULL;
    }
    
    User* dest = (User*)malloc(sizeof(User));
    if (!dest) {
        return NULL;
    }
    
    dest->user_id = strdup(source->user_id);
    dest->username = strdup(source->username);
    dest->nickname = strdup(source->nickname);
    dest->avatar_hash = strdup(source->avatar_hash);
    dest->signature = strdup(source->signature);
    dest->identity = source->identity;
    dest->tags = source->tags;
    dest->latitude = source->latitude;
    dest->longitude = source->longitude;
    dest->location_enabled = source->location_enabled;
    dest->last_seen = source->last_seen;
    dest->online = source->online;
    dest->exhibition_name = strdup(source->exhibition_name);
    dest->exhibition_url = strdup(source->exhibition_url);
    dest->followers = source->followers;
    dest->following = source->following;
    dest->emoji = strdup(source->emoji);
    
    return dest;
}

/**
 * @brief 更新用户信息
 * @param manager 用户管理器指针
 * @param user 用户信息
 * @return 更新是否成功
 */
bool user_manager_update_user(UserManager* manager, User* user) {
    if (!manager || !user || !manager->logged_in) {
        return false;
    }
    
    // 这里应该发送更新请求到服务器
    // 由于我们只是预留接口，这里只做简单的模拟
    printf("Updating user information for user: %s\n", user->username);
    
    // 更新本地用户信息
    if (manager->current_user) {
        // 释放旧资源
        free_user_resources(manager->current_user);
        
        // 复制新数据
        manager->current_user = copy_user_data(user);
        if (!manager->current_user) {
            fprintf(stderr, "Error allocating memory for user data\n");
            manager->logged_in = false;
            return false;
        }
    }
    
    return true;
}

/**
 * @brief 获取用户信息
 * @param manager 用户管理器指针
 * @param user_id 用户ID
 * @param callback 回调函数
 * @param user_data 用户数据
 * @return 请求是否发送成功
 */
bool user_manager_get_user_info(UserManager* manager, const char* user_id, LoginCallback callback, void* user_data) {
    if (!manager || !user_id) {
        return false;
    }
    
    // 这里应该发送获取用户信息请求到服务器
    // 由于我们只是预留接口，这里只做简单的模拟
    printf("Getting user information for user ID: %s\n", user_id);
    
    // 模拟获取用户信息
    // 实际应用中应该发送网络请求
    if (callback) {
        // 创建用户对象
        User temp_user;
        temp_user.user_id = (char*)user_id;
        temp_user.username = "test_user";
        temp_user.nickname = "测试用户";
        temp_user.avatar_hash = "avatar_hash_123";
        temp_user.signature = "这是一个测试签名";
        temp_user.identity = USER_IDENTITY_NORMAL;
        temp_user.tags = 0;
        temp_user.latitude = 39.9042;
        temp_user.longitude = 116.4074;
        temp_user.location_enabled = true;
        temp_user.last_seen = time(NULL);
        temp_user.online = true;
        temp_user.exhibition_name = "测试展会";
        temp_user.exhibition_url = "http://example.com/exhibition";
        temp_user.followers = 10;
        temp_user.following = 5;
        temp_user.emoji = "😊";
        
        // 复制用户数据
        User* user = copy_user_data(&temp_user);
        if (user) {
            // 调用回调函数
            callback(user_data, 200, user);
        } else {
            // 内存分配失败
            callback(user_data, 500, NULL);
        }
    }
    
    return true;
}

/**
 * @brief 生成设备指纹
 * @return 设备指纹字符串，失败返回 NULL
 */
char* user_manager_generate_device_fingerprint(void) {
    // 模拟生成设备指纹
    // 实际应用中应该根据设备硬件信息生成唯一的设备指纹
    char* fingerprint = (char*)malloc(64);
    if (fingerprint) {
        snprintf(fingerprint, 64, "device_%d", (int)time(NULL));
    }
    return fingerprint;
}

/**
 * @brief 检查用户是否登录
 * @param manager 用户管理器指针
 * @return 是否已登录
 */
bool user_manager_is_logged_in(UserManager* manager) {
    if (!manager) {
        return false;
    }
    return manager->logged_in;
}