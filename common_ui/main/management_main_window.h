#ifndef MANAGEMENT_MAIN_WINDOW_H
#define MANAGEMENT_MAIN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

// 管理端主窗口结构体
typedef struct {
    void* native_window;
    void* user_management;
    void* exhibition_management;
    void* transaction_audit;
    void* ticket_management;
    void* system_monitoring;
    void* security_management;
    void* user_info;
} ManagementMainWindow;

// 创建管理端主窗口
ManagementMainWindow* management_main_window_create(void);

// 销毁管理端主窗口
void management_main_window_destroy(ManagementMainWindow* window);

// 显示管理端主窗口
void management_main_window_show(ManagementMainWindow* window);

// 隐藏管理端主窗口
void management_main_window_hide(ManagementMainWindow* window);

// 设置用户信息
void management_main_window_set_user_info(ManagementMainWindow* window, const char* username, const char* nickname, const char* avatar);

// 切换到用户管理
void management_main_window_switch_to_users(ManagementMainWindow* window);

// 切换到展会管理
void management_main_window_switch_to_exhibition(ManagementMainWindow* window);

// 切换到交易审核
void management_main_window_switch_to_transaction_audit(ManagementMainWindow* window);

// 切换到门票管理
void management_main_window_switch_to_ticket(ManagementMainWindow* window);

// 切换到系统监控
void management_main_window_switch_to_system_monitoring(ManagementMainWindow* window);

// 切换到安全管理
void management_main_window_switch_to_security(ManagementMainWindow* window);

// 切换到用户信息
void management_main_window_switch_to_user_info(ManagementMainWindow* window);

#endif // MANAGEMENT_MAIN_WINDOW_H
