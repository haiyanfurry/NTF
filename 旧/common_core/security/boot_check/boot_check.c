#include "boot_check.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// 恶意程序特征
const char* malicious_signatures[] = {
    "virus", "malware", "trojan", "backdoor", "ransomware",
    "keylogger", "spyware", "adware", "rootkit", "worm",
    NULL
};

// 核心文件列表
const char* core_files[] = {
    "FindFriendCore.c", "FindFriendCore.h",
    "UserManager.c", "UserManager.h",
    "LocationManager.c", "LocationManager.h",
    "SocialManager.c", "SocialManager.h",
    "ExhibitionManager.c", "ExhibitionManager.h",
    "GameManager.c", "GameManager.h",
    "NetworkManager.c", "NetworkManager.h",
    "TLSSocket.c", "TLSSocket.h",
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

// 检查核心文件是否被篡改
int check_core_files(const char* directory) {
    if (directory == NULL) {
        return 0;
    }
    
    for (int i = 0; core_files[i] != NULL; i++) {
        char filepath[256];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, core_files[i]);
        
        if (access(filepath, F_OK) == -1) {
            return 1; // 核心文件缺失
        }
        
        // 这里可以添加文件哈希校验逻辑
        // 例如计算文件 MD5 并与预设值比较
    }
    
    return 0; // 核心文件完整
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

// 启动前检查
int boot_check(void) {
    // 扫描当前目录
    if (scan_directory(".")) {
        fprintf(stderr, "错误：检测到恶意文件，无法启动应用程序\n");
        return 1;
    }
    
    // 检查核心文件
    if (check_core_files("./core")) {
        fprintf(stderr, "错误：核心文件缺失或被篡改，无法启动应用程序\n");
        return 1;
    }
    
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