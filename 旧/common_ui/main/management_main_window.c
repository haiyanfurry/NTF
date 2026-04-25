#include "management_main_window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建管理端主窗口
ManagementMainWindow* management_main_window_create(void) {
    ManagementMainWindow* window = (ManagementMainWindow*)malloc(sizeof(ManagementMainWindow));
    if (!window) {
        fprintf(stderr, "Error allocating memory for management main window\n");
        return NULL;
    }
    
    // 初始化窗口组件
    window->native_window = NULL; // 实际应用中应该创建原生窗口
    window->user_management = NULL;
    window->exhibition_management = NULL;
    window->transaction_audit = NULL;
    window->ticket_management = NULL;
    window->system_monitoring = NULL;
    window->security_management = NULL;
    window->user_info = NULL;
    
    printf("Management main window created\n");
    return window;
}

// 销毁管理端主窗口
void management_main_window_destroy(ManagementMainWindow* window) {
    if (window) {
        // 释放窗口组件
        // 实际应用中应该销毁原生窗口和其他组件
        free(window);
        printf("Management main window destroyed\n");
    }
}

// 显示管理端主窗口
void management_main_window_show(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window shown\n");
    }
}

// 隐藏管理端主窗口
void management_main_window_hide(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window hidden\n");
    }
}

// 设置用户信息
void management_main_window_set_user_info(ManagementMainWindow* window, const char* username, const char* nickname, const char* avatar) {
    if (window) {
        printf("Management main window user info set: username=%s, nickname=%s, avatar=%s\n", username, nickname, avatar);
    }
}

// 切换到用户管理
void management_main_window_switch_to_users(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to user management\n");
    }
}

// 切换到展会管理
void management_main_window_switch_to_exhibition(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to exhibition management\n");
    }
}

// 切换到交易审核
void management_main_window_switch_to_transaction_audit(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to transaction audit\n");
    }
}

// 切换到门票管理
void management_main_window_switch_to_ticket(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to ticket management\n");
    }
}

// 切换到系统监控
void management_main_window_switch_to_system_monitoring(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to system monitoring\n");
    }
}

// 切换到安全管理
void management_main_window_switch_to_security(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to security management\n");
    }
}

// 切换到用户信息
void management_main_window_switch_to_user_info(ManagementMainWindow* window) {
    if (window) {
        printf("Management main window switched to user info\n");
    }
}
