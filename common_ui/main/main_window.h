#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 主窗口结构体
typedef struct {
    void* native_window;
    void* login_window;
    void* friend_list;
    void* chat_window;
    void* map_view;
} MainWindow;

// 创建主窗口
MainWindow* main_window_create(void);

// 销毁主窗口
void main_window_destroy(MainWindow* window);

// 显示主窗口
void main_window_show(MainWindow* window);

// 隐藏主窗口
void main_window_hide(MainWindow* window);

// 切换到登录界面
void main_window_switch_to_login(MainWindow* window);

// 切换到主界面
void main_window_switch_to_main(MainWindow* window);

// 显示好友列表
void main_window_show_friend_list(MainWindow* window);

// 显示聊天界面
void main_window_show_chat(MainWindow* window, const char* friend_id);

// 显示地图视图
void main_window_show_map(MainWindow* window);

#ifdef __cplusplus
}
#endif

#endif // MAIN_WINDOW_H