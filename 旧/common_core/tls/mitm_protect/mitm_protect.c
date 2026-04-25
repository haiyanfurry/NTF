#include "mitm_protect.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 服务器证书指纹（示例）
#define SERVER_CERT_FINGERPRINT "SHA256:1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef"

// 检查证书指纹
int check_certificate_fingerprint(const char* fingerprint) {
    if (fingerprint == NULL) {
        return 1; // 无效指纹
    }
    
    if (strcmp(fingerprint, SERVER_CERT_FINGERPRINT) != 0) {
        return 1; // 指纹不匹配
    }
    
    return 0; // 指纹匹配
}

// 检查是否使用代理
int check_proxy_hijack(void) {
    // 检查环境变量中的代理设置
    const char* http_proxy = getenv("http_proxy");
    const char* https_proxy = getenv("https_proxy");
    
    if (http_proxy != NULL || https_proxy != NULL) {
        return 1; // 检测到代理设置
    }
    
    return 0; // 未检测到代理
}

// 检查 SSL 剥离
int check_ssl_stripping(const char* url) {
    if (url == NULL) {
        return 0;
    }
    
    // 检查 URL 是否使用 HTTP 而非 HTTPS
    if (strstr(url, "http://") != NULL) {
        return 1; // 检测到 SSL 剥离
    }
    
    return 0; // 未检测到 SSL 剥离
}

// 检查数据篡改
int check_data_tampering(const char* original_data, const char* received_data, size_t length) {
    if (original_data == NULL || received_data == NULL) {
        return 1; // 无效数据
    }
    
    if (memcmp(original_data, received_data, length) != 0) {
        return 1; // 数据被篡改
    }
    
    return 0; // 数据未被篡改
}

// 综合防中间人攻击检查
int mitm_protect_check(const char* fingerprint, const char* url, const char* original_data, const char* received_data, size_t length) {
    // 检查证书指纹
    if (check_certificate_fingerprint(fingerprint)) {
        return 1;
    }
    
    // 检查代理劫持
    if (check_proxy_hijack()) {
        return 1;
    }
    
    // 检查 SSL 剥离
    if (check_ssl_stripping(url)) {
        return 1;
    }
    
    // 检查数据篡改
    if (check_data_tampering(original_data, received_data, length)) {
        return 1;
    }
    
    return 0; // 检查通过
}

// 初始化防中间人攻击模块
bool mitm_protect_init(void) {
    // 防中间人攻击模块不需要特殊初始化
    return true;
}

// 清理防中间人攻击模块
void mitm_protect_cleanup(void) {
    // 无资源需要释放
}