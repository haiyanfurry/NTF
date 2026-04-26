#include "device_fingerprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include <ctype.h>
#include <openssl/md5.h>
#include <openssl/ssl.h>

// 初始化设备指纹模块
bool device_fingerprint_init(void) {
    // 初始化OpenSSL
    SSL_load_error_strings();
    SSL_library_init();
    return true;
}

// 生成设备指纹
DeviceFingerprint* device_fingerprint_generate(void) {
    DeviceFingerprint* fingerprint = (DeviceFingerprint*)malloc(sizeof(DeviceFingerprint));
    if (!fingerprint) {
        fprintf(stderr, "Error allocating memory for device fingerprint\n");
        return NULL;
    }
    
    // 初始化结构体
    memset(fingerprint, 0, sizeof(DeviceFingerprint));
    
    // 收集设备信息
    struct utsname uname_info;
    if (uname(&uname_info) == 0) {
        snprintf(fingerprint->device_info, 512, "%s %s %s %s %s", 
                 uname_info.sysname, uname_info.nodename, 
                 uname_info.release, uname_info.version, 
                 uname_info.machine);
    }
    
    // 检测模拟器和虚拟机
    fingerprint->is_emulator = device_fingerprint_is_emulator();
    fingerprint->is_virtual_machine = device_fingerprint_is_virtual_machine();
    
    // 计算风险评分
    fingerprint->risk_score = device_fingerprint_calculate_risk();
    
    // 生成唯一指纹
    char data[1024];
    snprintf(data, 1024, "%s|%d|%ld|%d|%d", 
             fingerprint->device_info, 
             fingerprint->risk_score, 
             (long)time(NULL),
             fingerprint->is_emulator,
             fingerprint->is_virtual_machine);
    
    // 使用MD5生成指纹
    unsigned char md5_hash[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)data, strlen(data), md5_hash);
    
    // 转换为字符串
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&fingerprint->fingerprint[i*2], "%02x", md5_hash[i]);
    }
    
    return fingerprint;
}

// 验证设备指纹
bool device_fingerprint_validate(const char* fingerprint) {
    if (!fingerprint) {
        return false;
    }
    
    // 检查指纹长度
    if (strlen(fingerprint) != 32) {
        return false;
    }
    
    // 检查指纹格式
    for (int i = 0; i < 32; i++) {
        if (!isxdigit(fingerprint[i])) {
            return false;
        }
    }
    
    return true;
}

// 检测模拟器
bool device_fingerprint_is_emulator(void) {
    // 简单的模拟器检测逻辑
    // 实际应用中应该使用更复杂的检测方法
    return false;
}

// 检测虚拟机
bool device_fingerprint_is_virtual_machine(void) {
    // 简单的虚拟机检测逻辑
    // 实际应用中应该使用更复杂的检测方法
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char buffer[1024];
        while (fgets(buffer, 1024, fp)) {
            if (strstr(buffer, "VMware") || strstr(buffer, "VirtualBox") || strstr(buffer, "QEMU")) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }
    return false;
}

// 计算风险评分
int device_fingerprint_calculate_risk(void) {
    int risk = 0;
    
    // 模拟器检测
    if (device_fingerprint_is_emulator()) {
        risk += 80;
    }
    
    // 虚拟机检测
    if (device_fingerprint_is_virtual_machine()) {
        risk += 60;
    }
    
    // 时间因素（防止时间篡改）
    time_t now = time(NULL);
    if (now < 1600000000) { // 2020-09-13
        risk += 40;
    }
    
    return risk;
}

// 销毁设备指纹
void device_fingerprint_destroy(DeviceFingerprint* fingerprint) {
    if (fingerprint) {
        free(fingerprint);
    }
}

// 清理设备指纹模块
void device_fingerprint_cleanup(void) {
    // 无资源需要释放
}