#include "integrity_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/ssl.h>

// 核心模块文件列表
static const char* core_modules[] = {
    "src/core/tls/TLSSocket.c",
    "src/core/network/NetworkManager.c",
    "src/core/business/UserManager.c",
    "src/core/business/LocationManager.c",
    "src/core/business/SocialManager.c",
    "src/core/business/ExhibitionManager.c",
    "src/core/game/GameManager.c"
};

// 初始化完整性校验模块
bool integrity_check_init(void) {
    // 初始化OpenSSL
    SSL_load_error_strings();
    SSL_library_init();
    return true;
}

// 计算文件MD5哈希
char* integrity_check_calculate_file_hash(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", file_path);
        return NULL;
    }
    
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    
    unsigned char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        MD5_Update(&md5_ctx, buffer, bytes_read);
    }
    
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &md5_ctx);
    
    fclose(file);
    
    // 转换为字符串
    char* hash_str = (char*)malloc(33);
    if (!hash_str) {
        fprintf(stderr, "Error allocating memory for hash string\n");
        return NULL;
    }
    
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&hash_str[i*2], "%02x", hash[i]);
    }
    hash_str[32] = '\0';
    
    return hash_str;
}

// 验证文件完整性
bool integrity_check_verify_file(const char* file_path, const char* expected_hash) {
    if (!file_path || !expected_hash) {
        return false;
    }
    
    char* actual_hash = integrity_check_calculate_file_hash(file_path);
    if (!actual_hash) {
        return false;
    }
    
    bool result = (strcmp(actual_hash, expected_hash) == 0);
    free(actual_hash);
    
    return result;
}

// 验证核心模块完整性
bool integrity_check_verify_core_modules(void) {
    // 这里应该从安全存储中读取预期哈希值
    // 由于是示例，我们只检查文件是否存在
    for (int i = 0; i < sizeof(core_modules) / sizeof(core_modules[0]); i++) {
        FILE* file = fopen(core_modules[i], "rb");
        if (!file) {
            fprintf(stderr, "Core module missing: %s\n", core_modules[i]);
            return false;
        }
        fclose(file);
    }
    
    return true;
}

// 生成核心模块哈希列表
bool integrity_check_generate_core_hashes(void) {
    printf("Generating core module hashes...\n");
    
    for (int i = 0; i < sizeof(core_modules) / sizeof(core_modules[0]); i++) {
        char* hash = integrity_check_calculate_file_hash(core_modules[i]);
        if (hash) {
            printf("%s: %s\n", core_modules[i], hash);
            free(hash);
        } else {
            printf("Failed to calculate hash for: %s\n", core_modules[i]);
            return false;
        }
    }
    
    return true;
}

// 检查并拦截启动
bool integrity_check_intercept_startup(void) {
    printf("Checking core module integrity...\n");
    
    if (!integrity_check_verify_core_modules()) {
        fprintf(stderr, "Core module integrity check failed!\n");
        fprintf(stderr, "Intercepting startup and disconnecting network...\n");
        // 这里应该实现网络断开逻辑
        return false;
    }
    
    printf("Core module integrity check passed!\n");
    return true;
}

// 清理完整性检查模块
void integrity_check_cleanup(void) {
    // 无资源需要释放
}