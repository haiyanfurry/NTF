#include <stdio.h>
#include <stdlib.h>
#include "../common_core/find_friend_core.h"
#include "../common_ui/common_ui.h"

int main(int argc, char* argv[]) {
    printf("FindFriendApp Windows version starting...\n");
    
    // 初始化核心库
    if (find_friend_core_init("localhost", 8080) != 0) {
        fprintf(stderr, "Failed to initialize core module\n");
        return 1;
    }
    
    // 初始化UI模块
    common_ui_init();
    
    // 这里应该创建登录窗口并启动应用
    printf("App initialized successfully\n");
    
    // 模拟应用运行
    printf("App running...\n");
    
    // 销毁UI模块
    common_ui_destroy();
    
    // 销毁核心模块
    find_friend_core_destroy();
    
    printf("FindFriendApp Windows version exited\n");
    return 0;
}
