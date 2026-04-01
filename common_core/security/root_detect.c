#include "root_detect.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

// 检查是否以 root 权限运行
int check_root_privileges(void) {
    // 检查 UID 是否为 0（root）
    if (getuid() == 0) {
        return 1; // 以 root 权限运行
    }
    return 0; // 以普通用户权限运行
}

// 处理 root 权限检查
int handle_root_check(void) {
    if (check_root_privileges()) {
        fprintf(stderr, "错误：不允许以 root 权限运行 FindFriend App\n");
        fprintf(stderr, "请使用普通用户权限运行本程序\n");
        exit(1); // 直接退出程序
    }
    return 0; // 继续执行
}

// 初始化root检测模块
bool root_detect_init(void) {
    // 执行root权限检查
    return handle_root_check() == 0;
}

// 清理root检测模块
void root_detect_cleanup(void) {
    // 无资源需要释放
}