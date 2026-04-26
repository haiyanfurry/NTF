#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 登录回调函数类型
typedef void (*LoginWindowCallback)(void* user_data, int status, const char* username, const char* password, const char* device_fingerprint);

// 登录窗口结构体
typedef struct {
    void* native_window;
    LoginWindowCallback callback;
    void* user_data;
} LoginWindow;

// 创建登录窗口
LoginWindow* login_window_create(void* parent_window);

// 销毁登录窗口
void login_window_destroy(LoginWindow* window);

// 显示登录窗口
void login_window_show(LoginWindow* window);

// 隐藏登录窗口
void login_window_hide(LoginWindow* window);

// 设置登录回调
void login_window_set_callback(LoginWindow* window, LoginWindowCallback callback, void* user_data);

// 设置错误信息
void login_window_set_error(LoginWindow* window, const char* error_message);

#ifdef __cplusplus
}
#endif

#endif // LOGIN_WINDOW_H