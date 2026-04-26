#include "main_window.h"
#include <stdio.h>
#include <stdlib.h>

// 创建主窗口
MainWindow* main_window_create(void) {
    MainWindow* window = (MainWindow*)malloc(sizeof(MainWindow));
    if (!window) {
        fprintf(stderr, "Error allocating memory for main window\n");
        return NULL;
    }
    
    // 初始化结构体
    window->native_window = NULL; // 由平台特定实现设置
    window->login_window = NULL;
    window->friend_list = NULL;
    window->chat_window = NULL;
    window->map_view = NULL;
    
    return window;
}

// 销毁主窗口
void main_window_destroy(MainWindow* window) {
    if (window) {
        // 平台特定实现应该释放 native_window 和其他子窗口
        free(window);
    }
}

// 显示主窗口
void main_window_show(MainWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing main window\n");
}

// 隐藏主窗口
void main_window_hide(MainWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Hiding main window\n");
}

// 切换到登录界面
void main_window_switch_to_login(MainWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Switching to login interface\n");
}

// 切换到主界面
void main_window_switch_to_main(MainWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Switching to main interface\n");
}

// 显示好友列表
void main_window_show_friend_list(MainWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing friend list\n");
}

// 显示聊天界面
void main_window_show_chat(MainWindow* window, const char* friend_id) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing chat interface for friend: %s\n", friend_id);
}

// 显示地图视图
void main_window_show_map(MainWindow* window) {
    if (!window) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing map view\n");
}