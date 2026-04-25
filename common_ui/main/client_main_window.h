#ifndef CLIENT_MAIN_WINDOW_H
#define CLIENT_MAIN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

// 客户端主窗口结构体
typedef struct {
    void* native_window;
    void* friend_list;
    void* chat_window;
    void* map_view;
    void* exhibition_view;
    void* ticket_view;
    void* media_transfer;
    void* message_view;
    void* user_info;
} ClientMainWindow;

// 创建客户端主窗口
ClientMainWindow* client_main_window_create(void);

// 销毁客户端主窗口
void client_main_window_destroy(ClientMainWindow* window);

// 显示客户端主窗口
void client_main_window_show(ClientMainWindow* window);

// 隐藏客户端主窗口
void client_main_window_hide(ClientMainWindow* window);

// 设置用户信息
void client_main_window_set_user_info(ClientMainWindow* window, const char* username, const char* nickname, const char* avatar);

// 切换到好友列表
void client_main_window_switch_to_friends(ClientMainWindow* window);

// 切换到聊天界面
void client_main_window_switch_to_chat(ClientMainWindow* window, const char* friend_id);

// 切换到地图界面
void client_main_window_switch_to_map(ClientMainWindow* window);

// 切换到展会浏览
void client_main_window_switch_to_exhibition(ClientMainWindow* window);

// 切换到入场码
void client_main_window_switch_to_ticket(ClientMainWindow* window);

// 切换到媒体传输
void client_main_window_switch_to_media_transfer(ClientMainWindow* window);

// 切换到消息管理
void client_main_window_switch_to_message(ClientMainWindow* window);

// 切换到用户信息
void client_main_window_switch_to_user_info(ClientMainWindow* window);

#endif // CLIENT_MAIN_WINDOW_H
