#ifndef FIND_FRIEND_CORE_H
#define FIND_FRIEND_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

// 包含核心模块头文件
#include "version/version.h"
#include "tls/tls_socket.h"
#include "network/network.h"
#include "business/user_manager.h"
#include "business/location_manager.h"
#include "business/social_manager.h"
#include "business/exhibition_manager.h"
#include "security/security.h"

// 初始化核心系统
bool find_friend_core_init(const char* server_hostname, int server_port);

// 销毁核心系统
void find_friend_core_destroy();

// 获取当前服务器IP
const char* find_friend_core_get_current_server_ip();

// 获取用户管理器
UserManager* find_friend_core_get_user_manager();

// 获取定位管理器
LocationManager* find_friend_core_get_location_manager();

// 获取社交管理器
SocialManager* find_friend_core_get_social_manager();

// 获取展商管理器
ExhibitionManager* find_friend_core_get_exhibition_manager();

#ifdef __cplusplus
}
#endif

#endif // FIND_FRIEND_CORE_H