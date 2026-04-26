#ifndef COMMON_UI_H
#define COMMON_UI_H

#include "login/login_window.h"
#include "main/main_window.h"
#include "main/pc_main_window.h"
#include "main/merchant_main_window.h"
#include "main/client_main_window.h"
#include "main/management_main_window.h"
#include "friend_list/friend_list.h"
#include "chat/chat_window.h"
#include "map/map_view.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初始化公共UI组件
void common_ui_init(void);

// 销毁公共UI组件
void common_ui_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // COMMON_UI_H