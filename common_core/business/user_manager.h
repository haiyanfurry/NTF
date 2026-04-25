#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 用户结构体
typedef struct {
    char* user_id;
    char* username;
    char* nickname;
    char* avatar_hash;
    char* signature;
    int identity;
    int tags;
    double latitude;
    double longitude;
    bool location_enabled;
    int64_t last_seen;
    bool online;
    char* exhibition_name;
    char* exhibition_url;
    int followers;
    int following;
    char* emoji;
} User;

// 用户身份枚举
typedef enum {
    USER_IDENTITY_NORMAL = 0,    // 普通用户
    USER_IDENTITY_HAS_BADGE = 1,  // 有装用户
    USER_IDENTITY_OFFICIAL = 2    // 官方用户
} UserIdentity;

// 登录回调函数类型
typedef void (*LoginCallback)(void* user_data, int status, User* user);

// 用户管理器结构体
typedef struct {
    void* network_manager;
    User* current_user;
    bool logged_in;
} UserManager;

// 初始化用户管理器
UserManager* user_manager_init();

// 销毁用户管理器
void user_manager_destroy(UserManager* manager);

// 登录
bool user_manager_login(UserManager* manager, const char* username, const char* password, const char* device_fingerprint, LoginCallback callback, void* user_data);

// 登出
void user_manager_logout(UserManager* manager);

// 获取当前用户
User* user_manager_get_current_user(UserManager* manager);

// 更新用户信息
bool user_manager_update_user(UserManager* manager, User* user);

// 获取用户信息
bool user_manager_get_user_info(UserManager* manager, const char* user_id, LoginCallback callback, void* user_data);

// 生成设备指纹
char* user_manager_generate_device_fingerprint(void);

// 检查用户是否登录
bool user_manager_is_logged_in(UserManager* manager);

#ifdef __cplusplus
}
#endif

#endif // USER_MANAGER_H