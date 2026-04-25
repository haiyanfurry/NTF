#include "security.h"
#include "device_fingerprint.h"
#include "memory_encrypt.h"
#include "integrity_check.h"
#include "boot_check/boot_check.h"
#include "root_detect.h"
#include "inject_filter.h"
#include "../tls/mitm_protect/mitm_protect.h"
#include "dos_protect.h"
#include "xml_protect.h"
#include "linux_desktop_compat.h"
#include <stdio.h>

// 函数声明
bool boot_check_init(void);
void boot_check_cleanup(void);
bool root_detect_init(void);
void root_detect_cleanup(void);
bool inject_filter_init(void);
void inject_filter_cleanup(void);
bool mitm_protect_init(void);
void mitm_protect_cleanup(void);
bool dos_protect_init(void);
void dos_protect_cleanup(void);
bool integrity_check_init(void);
void integrity_check_cleanup(void);
bool memory_encrypt_init(void);
void memory_encrypt_cleanup(void);
bool device_fingerprint_init(void);
void device_fingerprint_cleanup(void);

// 安全模块状态
static bool security_initialized = false;

// 唯一全局安全初始化函数
void security_init(void) {
    printf("Initializing all security modules...\n");
    
    // 初始化设备指纹
    if (device_fingerprint_init()) {
        printf("Device fingerprint initialized\n");
    } else {
        printf("Failed to initialize device fingerprint\n");
    }
    
    // 初始化内存加密
    if (memory_encrypt_init()) {
        printf("Memory encryption initialized\n");
    } else {
        printf("Failed to initialize memory encryption\n");
    }
    
    // 初始化完整性校验
    if (integrity_check_init()) {
        printf("Integrity check initialized\n");
    } else {
        printf("Failed to initialize integrity check\n");
    }
    
    // 初始化启动自检
    if (boot_check_init()) {
        printf("Boot check initialized\n");
    } else {
        printf("Failed to initialize boot check\n");
    }
    
    // 初始化root检测
    if (root_detect_init()) {
        printf("Root detect initialized\n");
    } else {
        printf("Failed to initialize root detect\n");
    }
    
    // 初始化防注入
    if (inject_filter_init()) {
        printf("Inject filter initialized\n");
    } else {
        printf("Failed to initialize inject filter\n");
    }
    
    // 初始化防MITM
    if (mitm_protect_init()) {
        printf("MITM protection initialized\n");
    } else {
        printf("Failed to initialize MITM protection\n");
    }
    
    // 初始化防DoS
    if (dos_protect_init()) {
        printf("DoS protection initialized\n");
    } else {
        printf("Failed to initialize DoS protection\n");
    }
    
    // 初始化XML保护
    if (xml_protect_init()) {
        printf("XML protection initialized\n");
    } else {
        printf("Failed to initialize XML protection\n");
    }
    
    // 初始化Linux桌面兼容
    if (linux_desktop_compat_init()) {
        printf("Linux desktop compatibility initialized\n");
    } else {
        printf("Failed to initialize Linux desktop compatibility\n");
    }
    
    security_initialized = true;
    printf("All security modules initialized successfully\n");
}

// 安全模块状态检查
bool security_check_status(void) {
    return security_initialized;
}

// 安全模块清理函数
void security_cleanup(void) {
    printf("Cleaning up security modules...\n");
    
    // 清理Linux桌面兼容
    linux_desktop_compat_cleanup();
    
    // 清理XML保护
    xml_protect_cleanup();
    
    // 清理防DoS
    dos_protect_cleanup();
    
    // 清理防MITM
    mitm_protect_cleanup();
    
    // 清理防注入
    inject_filter_cleanup();
    
    // 清理root检测
    root_detect_cleanup();
    
    // 清理启动自检
    boot_check_cleanup();
    
    // 清理完整性校验
    integrity_check_cleanup();
    
    // 清理内存加密
    memory_encrypt_cleanup();
    
    // 清理设备指纹
    device_fingerprint_cleanup();
    
    security_initialized = false;
    printf("All security modules cleaned up successfully\n");
}