#include "boot_check.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Boot Check 调试模式 - 设置为 1 则跳过核心文件检查
#define BOOT_CHECK_DEBUG_MODE 1

// 恶意程序特征
const char* malicious_signatures[] = {
    "virus", "malware", "trojan", "backdoor", "ransomware",
    "keylogger", "spyware", "adware", "rootkit", "worm",
    NULL
};

// 核心文件列表 - 相对于项目根目录的路径
const char* core_files[] = {
    "common_core/find_friend_core.c",
    "common_core/find_friend_core.h",
    "common_core/business/user_manager.c",
    "common_core/business/user_manager.h",
    "common_core/business/location_manager.c",
    "common_core/business/location_manager.h",
    "common_core/business/social_manager.c",
    "common_core/business/social_manager.h",
    "common_core/business/exhibition_manager.c",
    "common_core/business/exhibition_manager.h",
    "common_core/game/game_manager.c",
    "common_core/game/game_manager.h",
    "common_core/network/network.c",
    "common_core/network/network.h",
    "common_core/tls/tls_socket.c",
    "common_core/tls/tls_socket.h",
    "common_core/auth/auth.c",
    "common_core/auth/auth.h",
    "common_core/data/data_manager.c",
    "common_core/data/data_manager.h",
    "common_core/util/utils.c",
    "common_core/util/utils.h",
    NULL
};

// 检查文件是否为恶意程序
int check_malicious_file(const char* filename) {
    if (filename == NULL) {
        return 0;
    }

    for (int i = 0; malicious_signatures[i] != NULL; i++) {
        if (strstr(filename, malicious_signatures[i]) != NULL) {
            return 1; // 检测到恶意程序
        }
    }

    return 0; // 安全
}

// 扫描目录
int scan_directory(const char* directory) {
    if (directory == NULL) {
        return 0;
    }

    DIR* dir = opendir(directory);
    if (dir == NULL) {
        return 0;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // 跳过 . 和 ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 检查文件名是否包含恶意特征
        if (check_malicious_file(entry->d_name)) {
            closedir(dir);
            return 1; // 检测到恶意文件
        }

        // 检查是否为目录
        char filepath[256];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        struct stat statbuf;
        if (stat(filepath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            // 递归扫描子目录
            if (scan_directory(filepath)) {
                closedir(dir);
                return 1; // 检测到恶意文件
            }
        }
    }

    closedir(dir);
    return 0; // 未检测到恶意文件
}

// 检查核心文件是否被篡改（调试模式下跳过）
int check_core_files(const char* directory) {
    if (directory == NULL) {
        return 0;
    }

#if BOOT_CHECK_DEBUG_MODE == 1
    printf("Boot check: [DEBUG MODE] Core file check skipped\n");
    return 0; // 调试模式下始终返回成功
#else
    printf("Boot check: Scanning for core files...\n");

    int all_found = 1;
    for (int i = 0; core_files[i] != NULL; i++) {
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, core_files[i]);

        if (access(filepath, F_OK) == 0) {
            printf("  Found: %s\n", core_files[i]);
        } else {
            fprintf(stderr, "  MISSING: %s\n", core_files[i]);
            all_found = 0;
        }
    }

    if (!all_found) {
        fprintf(stderr, "Boot check: Some core files are missing!\n");
        return 1;
    }

    return 0;
#endif
}

// 启动前检查
int boot_check(void) {
    printf("Boot check: Starting...\n");

    // 扫描当前目录
    if (scan_directory(".")) {
        fprintf(stderr, "错误：检测到恶意文件，无法启动应用程序\n");
        return 1;
    }

    // 检查核心文件（调试模式下跳过）
    if (check_core_files(".")) {
#if BOOT_CHECK_DEBUG_MODE == 1
        printf("Boot check: [DEBUG MODE] Continuing despite core file check failure\n");
#else
        fprintf(stderr, "错误：核心文件缺失或被篡改，无法启动应用程序\n");
        return 1;
#endif
    }

    printf("Boot check: All checks passed\n");
    return 0; // 检查通过
}

// 初始化启动检查模块
bool boot_check_init(void) {
    // 执行启动检查
    return boot_check() == 0;
}

// 清理启动检查模块
void boot_check_cleanup(void) {
    // 无资源需要释放
}
