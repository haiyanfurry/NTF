#ifndef AUTH_H
#define AUTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 认证状态枚举
typedef enum {
    AUTH_STATUS_NOT_AUTHENTICATED = 0,  // 未认证
    AUTH_STATUS_AUTHENTICATING = 1,     // 认证中
    AUTH_STATUS_AUTHENTICATED = 2,      // 已认证
    AUTH_STATUS_FAILED = 3              // 认证失败
} AuthStatus;

// 认证错误码
typedef enum {
    AUTH_ERROR_NONE = 0,
    AUTH_ERROR_INVALID_CREDENTIALS = 1,
    AUTH_ERROR_NETWORK = 2,
    AUTH_ERROR_TIMEOUT = 3,
    AUTH_ERROR_SERVER = 4,
    AUTH_ERROR_UNKNOWN = 5
} AuthError;

// 认证令牌结构体
typedef struct {
    char* access_token;
    char* refresh_token;
    char* token_type;
    int64_t expires_at;
    char* scope;
} AuthToken;

// 认证回调函数类型
typedef void (*AuthCallback)(void* user_data, AuthStatus status, AuthError error, AuthToken* token);

// 认证管理器结构体
typedef struct {
    AuthToken* current_token;
    AuthStatus status;
    AuthError last_error;
    void* network_manager;
} AuthManager;

// 初始化认证管理器
AuthManager* auth_manager_init();

// 销毁认证管理器
void auth_manager_destroy(AuthManager* manager);

// 用户名密码认证
bool auth_manager_authenticate(AuthManager* manager, const char* username, const char* password, AuthCallback callback, void* user_data);

// 使用刷新令牌刷新认证
bool auth_manager_refresh_token(AuthManager* manager, AuthCallback callback, void* user_data);

// 登出
void auth_manager_logout(AuthManager* manager);

// 获取当前认证状态
AuthStatus auth_manager_get_status(AuthManager* manager);

// 获取当前认证令牌
AuthToken* auth_manager_get_token(AuthManager* manager);

// 检查认证是否有效
bool auth_manager_is_token_valid(AuthManager* manager);

// 保存认证令牌到本地
bool auth_manager_save_token(AuthManager* manager, const char* file_path);

// 从本地加载认证令牌
bool auth_manager_load_token(AuthManager* manager, const char* file_path);

#ifdef __cplusplus
}
#endif

#endif // AUTH_H
