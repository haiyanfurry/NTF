// FindFriend Linux 平台主入口

#include "ff_core_interface.h"
#include "ff_user_interface.h"
#include "ff_message_interface.h"
#include "ff_location_interface.h"
#include "ff_p2p_interface.h"
#include "ff_game_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    FFResult res;
    
    // 初始化核心模块
    res = ff_core_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize core: %d\n", res);
        return 1;
    }
    
    // 初始化用户管理
    res = ff_user_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize user manager: %d\n", res);
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化消息管理
    res = ff_message_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize message manager: %d\n", res);
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化社交管理
    res = ff_social_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize social manager: %d\n", res);
        ff_message_cleanup();
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化游戏管理
    res = ff_game_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize game manager: %d\n", res);
        ff_social_cleanup();
        ff_message_cleanup();
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化位置服务
    res = ff_location_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize location service: %d\n", res);
        ff_game_cleanup();
        ff_social_cleanup();
        ff_message_cleanup();
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化 P2P 模块
    res = ff_p2p_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize P2P module: %d\n", res);
        ff_location_cleanup();
        ff_game_cleanup();
        ff_social_cleanup();
        ff_message_cleanup();
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化数据存储
    res = ff_data_store_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize data store: %d\n", res);
        ff_p2p_cleanup();
        ff_location_cleanup();
        ff_game_cleanup();
        ff_social_cleanup();
        ff_message_cleanup();
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    // 初始化缓存管理
    res = ff_cache_manager_init(1024 * 1024); // 1MB
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize cache manager: %d\n", res);
        ff_data_store_cleanup();
        ff_p2p_cleanup();
        ff_location_cleanup();
        ff_game_cleanup();
        ff_social_cleanup();
        ff_message_cleanup();
        ff_user_cleanup();
        ff_core_cleanup();
        return 1;
    }
    
    printf("FindFriend initialized successfully!\n");
    printf("Use ff_cli tool for command-line operations.\n");
    
    // 主循环
    while (1) {
        sleep(1);
    }
    
    // 清理资源
    ff_cache_manager_cleanup();
    ff_data_store_cleanup();
    ff_p2p_cleanup();
    ff_location_cleanup();
    ff_game_cleanup();
    ff_social_cleanup();
    ff_message_cleanup();
    ff_user_cleanup();
    ff_core_cleanup();
    
    return 0;
}
