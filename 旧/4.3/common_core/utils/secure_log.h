#ifndef SECURE_LOG_H
#define SECURE_LOG_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// 日志级别
typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
} LogLevel;

// 日志类型
typedef enum {
    LOG_TYPE_GENERAL = 0,
    LOG_TYPE_SECURITY,
    LOG_TYPE_NETWORK,
    LOG_TYPE_DATABASE,
    LOG_TYPE_USER_ACTION,
    LOG_TYPE_SYSTEM
} LogType;

// 敏感信息类型
typedef enum {
    SENSITIVE_NONE = 0,
    SENSITIVE_PASSWORD,
    SENSITIVE_TOKEN,
    SENSITIVE_IP,
    SENSITIVE_EMAIL,
    SENSITIVE_PHONE,
    SENSITIVE_ID_CARD,
    SENSITIVE_CREDIT_CARD,
    SENSITIVE_PRIVATE_KEY,
    SENSITIVE_SESSION_ID
} SensitiveType;

// 初始化安全日志系统
bool secure_log_init(const char* log_path, LogLevel min_level);

// 清理安全日志系统
void secure_log_cleanup(void);

// 设置日志级别
void secure_log_set_level(LogLevel level);

// 获取日志级别
LogLevel secure_log_get_level(void);

// 安全日志记录（自动过滤敏感信息）
void secure_log(LogLevel level, LogType type, const char* file, int line, const char* format, ...);

// 安全日志记录（带敏感信息标记）
void secure_log_sensitive(LogLevel level, LogType type, const char* file, int line, 
                          SensitiveType sensitive_type, const char* format, ...);

// 加密并写入日志
bool secure_log_write_encrypted(const char* message, size_t len);

// 解密日志文件
bool secure_log_decrypt_file(const char* encrypted_path, const char* decrypted_path);

// 检查字符串是否包含敏感信息
SensitiveType secure_log_detect_sensitive(const char* str);

// 过滤敏感信息
char* secure_log_filter_sensitive(const char* input, char* output, size_t output_size);

// 日志轮转
bool secure_log_rotate(void);

// 获取日志文件大小
size_t secure_log_get_file_size(void);

// 获取日志文件路径
const char* secure_log_get_path(void);

// 日志宏定义
#define SECURE_LOG_ERROR(type, ...)   secure_log(LOG_LEVEL_ERROR, type, __FILE__, __LINE__, __VA_ARGS__)
#define SECURE_LOG_WARNING(type, ...) secure_log(LOG_LEVEL_WARNING, type, __FILE__, __LINE__, __VA_ARGS__)
#define SECURE_LOG_INFO(type, ...)    secure_log(LOG_LEVEL_INFO, type, __FILE__, __LINE__, __VA_ARGS__)
#define SECURE_LOG_DEBUG(type, ...)   secure_log(LOG_LEVEL_DEBUG, type, __FILE__, __LINE__, __VA_ARGS__)
#define SECURE_LOG_VERBOSE(type, ...) secure_log(LOG_LEVEL_VERBOSE, type, __FILE__, __LINE__, __VA_ARGS__)

// 安全日志宏（敏感信息自动过滤）
#define SECURE_LOG_SENSITIVE(level, type, sensitive_type, ...) \
    secure_log_sensitive(level, type, __FILE__, __LINE__, sensitive_type, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* SECURE_LOG_H */
