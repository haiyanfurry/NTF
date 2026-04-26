// FindFriend NetBSD 平台主入口

#include "platform_specific.h"
#include "ff_core_interface.h"
#include "ff_user_interface.h"
#include "ff_message_interface.h"
#include "ff_location_interface.h"
#include "ff_p2p_interface.h"
#include "ff_security_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static bool g_running = true;

static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        g_running = false;
        printf("\nReceived signal, shutting down...\n");
    }
}

int main(int argc, char* argv[]) {
    printf("FindFriend NetBSD platform\n");
    printf("Version: 5.0\n");
    printf("PID: %d\n", getpid());
    printf("Platform: NetBSD\n");
    
    // 注册信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // 初始化平台
    if (platform_init() != FF_OK) {
        fprintf(stderr, "Failed to initialize platform\n");
        return 1;
    }
    
    // 初始化核心
    if (ff_core_init() != FF_OK) {
        fprintf(stderr, "Failed to initialize core\n");
        platform_cleanup();
        return 1;
    }
    
    // 初始化 P2P
    if (ff_p2p_init(NULL, NULL) != FF_OK) {
        fprintf(stderr, "Failed to initialize P2P\n");
    }
    
    // 初始化安全模块
    if (ff_security_init() != FF_OK) {
        fprintf(stderr, "Failed to initialize security\n");
    }
    
    printf("FindFriend initialized successfully\n");
    printf("P2P node running...\n");
    printf("Press Ctrl+C to exit\n");
    
    // 主循环
    while (g_running) {
        // 处理 P2P 事件
        platform_poll_events();
        
        // 模拟工作
        usleep(100000); // 100ms
    }
    
    printf("\nShutting down...\n");
    
    // 清理
    ff_p2p_shutdown();
    ff_security_destroy();
    ff_core_destroy();
    platform_cleanup();
    
    printf("FindFriend exited\n");
    return 0;
}
