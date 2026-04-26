#ifndef INTEGRITY_CHECK_H
#define INTEGRITY_CHECK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 完整性校验结构体
typedef struct {
    char file_path[256];
    char expected_hash[33];
    bool verified;
} IntegrityCheck;

// 初始化完整性校验模块
bool integrity_check_init(void);

// 计算文件MD5哈希
char* integrity_check_calculate_file_hash(const char* file_path);

// 验证文件完整性
bool integrity_check_verify_file(const char* file_path, const char* expected_hash);

// 验证核心模块完整性
bool integrity_check_verify_core_modules(void);

// 生成核心模块哈希列表
bool integrity_check_generate_core_hashes(void);

// 检查并拦截启动
bool integrity_check_intercept_startup(void);

#ifdef __cplusplus
}
#endif

#endif // INTEGRITY_CHECK_H