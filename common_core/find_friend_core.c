#include "find_friend_core.h"
#include "bridge/bridge.h"
#include <stdio.h>
#include <stdlib.h>

// 核心管理器实例
static UserManager* user_manager = NULL;
static LocationManager* location_manager = NULL;
static SocialManager* social_manager = NULL;
static ExhibitionManager* exhibition_manager = NULL;

// 初始化核心系统
bool find_friend_core_init(const char* server_hostname, int server_port) {
    // 参数校验
    if (!server_hostname) {
        fprintf(stderr, "Error: server hostname is NULL\n");
        return false;
    }
    
    // 初始化所有安全模块
    security_init();
    if (!security_check_status()) {
        fprintf(stderr, "Error initializing security module\n");
        return false;
    }
    
    // 初始化桥接层
    bridge_init();
    
    // 初始化网络模块
    if (network_init() != NETWORK_SUCCESS) {
        fprintf(stderr, "Error initializing network module\n");
        bridge_cleanup();
        security_cleanup();
        return false;
    }
    
    // 设置服务器IP
    if (network_set_server_ip(server_hostname) != NETWORK_SUCCESS) {
        fprintf(stderr, "Error setting server IP\n");
        network_close();
        bridge_cleanup();
        security_cleanup();
        return false;
    }
    
    // 初始化用户管理器
    user_manager = user_manager_init();
    if (!user_manager) {
        fprintf(stderr, "Error initializing user manager\n");
        network_close();
        bridge_cleanup();
        security_cleanup();
        return false;
    }
    
    // 初始化定位管理器
    location_manager = location_manager_init();
    if (!location_manager) {
        fprintf(stderr, "Error initializing location manager\n");
        user_manager_destroy(user_manager);
        user_manager = NULL;
        network_close();
        bridge_cleanup();
        security_cleanup();
        return false;
    }
    
    // 初始化社交管理器
    social_manager = social_manager_init();
    if (!social_manager) {
        fprintf(stderr, "Error initializing social manager\n");
        location_manager_destroy(location_manager);
        location_manager = NULL;
        user_manager_destroy(user_manager);
        user_manager = NULL;
        network_close();
        bridge_cleanup();
        security_cleanup();
        return false;
    }
    
    // 初始化展商管理器
    exhibition_manager = exhibition_manager_init();
    if (!exhibition_manager) {
        fprintf(stderr, "Error initializing exhibition manager\n");
        social_manager_destroy(social_manager);
        social_manager = NULL;
        location_manager_destroy(location_manager);
        location_manager = NULL;
        user_manager_destroy(user_manager);
        user_manager = NULL;
        network_close();
        bridge_cleanup();
        security_cleanup();
        return false;
    }
    
    return true;
}

// 销毁核心系统
void find_friend_core_destroy() {
    // 销毁展商管理器
    if (exhibition_manager) {
        exhibition_manager_destroy(exhibition_manager);
        exhibition_manager = NULL;
    }
    
    // 销毁社交管理器
    if (social_manager) {
        social_manager_destroy(social_manager);
        social_manager = NULL;
    }
    
    // 销毁定位管理器
    if (location_manager) {
        location_manager_destroy(location_manager);
        location_manager = NULL;
    }
    
    // 销毁用户管理器
    if (user_manager) {
        user_manager_destroy(user_manager);
        user_manager = NULL;
    }
    
    // 关闭网络模块
    network_close();
    
    // 清理桥接层
    bridge_cleanup();
    
    // 清理安全模块
    security_cleanup();
}

// 获取当前服务器IP
const char* find_friend_core_get_current_server_ip() {
    return network_get_current_server_ip();
}

// 获取用户管理器
UserManager* find_friend_core_get_user_manager() {
    return user_manager;
}

// 获取定位管理器
LocationManager* find_friend_core_get_location_manager() {
    return location_manager;
}

// 获取社交管理器
SocialManager* find_friend_core_get_social_manager() {
    return social_manager;
}

// 获取展商管理器
ExhibitionManager* find_friend_core_get_exhibition_manager() {
    return exhibition_manager;
}