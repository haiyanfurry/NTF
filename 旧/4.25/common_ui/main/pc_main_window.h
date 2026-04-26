#ifndef PC_MAIN_WINDOW_H
#define PC_MAIN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

// PC端主窗口结构体
typedef struct {
    void* native_window;
    void* friend_list;
    void* chat_window;
    void* map_view;
    void* exhibition_view;
    void* ticket_view;
    void* transaction_view;
    void* media_transfer_view;
    void* message_view;
    void* user_info;
} PCMainWindow;

// 创建PC端主窗口
PCMainWindow* pc_main_window_create(void);

// 销毁PC端主窗口
void pc_main_window_destroy(PCMainWindow* window);

// 显示PC端主窗口
void pc_main_window_show(PCMainWindow* window);

// 隐藏PC端主窗口
void pc_main_window_hide(PCMainWindow* window);

// 设置用户信息
void pc_main_window_set_user_info(PCMainWindow* window, const char* username, const char* nickname, const char* avatar);

// 切换到好友列表
void pc_main_window_switch_to_friends(PCMainWindow* window);

// 切换到聊天界面
void pc_main_window_switch_to_chat(PCMainWindow* window, const char* friend_id);

// 切换到地图界面
void pc_main_window_switch_to_map(PCMainWindow* window);

// 切换到展会管理
void pc_main_window_switch_to_exhibition(PCMainWindow* window);

// 切换到入场码管理
void pc_main_window_switch_to_ticket(PCMainWindow* window);

// 切换到交易管理
void pc_main_window_switch_to_transaction(PCMainWindow* window);

// 切换到媒体传输
void pc_main_window_switch_to_media_transfer(PCMainWindow* window);

// 切换到消息管理
void pc_main_window_switch_to_message(PCMainWindow* window);

// 切换到用户信息
void pc_main_window_switch_to_user_info(PCMainWindow* window);

#endif // PC_MAIN_WINDOW_H
