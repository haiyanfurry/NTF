#include "secure_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <regex.h>

// 日志文件句柄
static FILE* g_log_file = NULL;
static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
static LogLevel g_min_level = LOG_LEVEL_INFO;
static char g_log_path[256] = {0};
static size_t g_max_file_size = 10 * 1024 * 1024; // 10MB

// AES加密密钥（实际应用中应该从安全存储获取）
static const uint8_t g_aes_key[32] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
};

static const uint8_t g_aes_iv[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

// 日志级别字符串
static const char* log_level_str[] = {
    "NONE", "ERROR", "WARNING", "INFO", "DEBUG", "VERBOSE"
};

// 日志类型字符串
static const char* log_type_str[] = {
    "GENERAL", "SECURITY", "NETWORK", "DATABASE", "USER", "SYSTEM"
};

// 初始化安全日志系统
bool secure_log_init(const char* log_path, LogLevel min_level) {
    pthread_mutex_lock(&g_log_mutex);
    
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    
    if (log_path) {
        strncpy(g_log_path, log_path, sizeof(g_log_path) - 1);
        g_log_file = fopen(log_path, "a");
        if (!g_log_file) {
            pthread_mutex_unlock(&g_log_mutex);
            return false;
        }
    }
    
    g_min_level = min_level;
    
    pthread_mutex_unlock(&g_log_mutex);
    
    SECURE_LOG_INFO(LOG_TYPE_SYSTEM, "Secure logging system initialized");
    return true;
}

// 清理安全日志系统
void secure_log_cleanup(void) {
    pthread_mutex_lock(&g_log_mutex);
    
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    
    pthread_mutex_unlock(&g_log_mutex);
}

// 设置日志级别
void secure_log_set_level(LogLevel level) {
    pthread_mutex_lock(&g_log_mutex);
    g_min_level = level;
    pthread_mutex_unlock(&g_log_mutex);
}

// 获取日志级别
LogLevel secure_log_get_level(void) {
    return g_min_level;
}

// 检查字符串是否包含敏感信息
SensitiveType secure_log_detect_sensitive(const char* str) {
    if (!str) return SENSITIVE_NONE;
    
    // 密码检测
    if (strstr(str, "password") || strstr(str, "passwd") || 
        strstr(str, "pwd") || strstr(str, "密钥")) {
        return SENSITIVE_PASSWORD;
    }
    
    // Token检测
    if (strstr(str, "token") || strstr(str, "Token") || 
        strstr(str, "TOKEN")) {
        return SENSITIVE_TOKEN;
    }
    
    // IP地址检测（简单检测）
    regex_t regex;
    if (regcomp(&regex, "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$", REG_EXTENDED) == 0) {
        if (regexec(&regex, str, 0, NULL, 0) == 0) {
            regfree(&regex);
            return SENSITIVE_IP;
        }
        regfree(&regex);
    }
    
    // 邮箱检测
    if (regcomp(&regex, "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}", REG_EXTENDED) == 0) {
        if (regexec(&regex, str, 0, NULL, 0) == 0) {
            regfree(&regex);
            return SENSITIVE_EMAIL;
        }
        regfree(&regex);
    }
    
    // 手机号检测（中国大陆）
    if (regcomp(&regex, "1[3-9][0-9]{9}", REG_EXTENDED) == 0) {
        if (regexec(&regex, str, 0, NULL, 0) == 0) {
            regfree(&regex);
            return SENSITIVE_PHONE;
        }
        regfree(&regex);
    }
    
    // 身份证号检测（简单检测18位）
    if (regcomp(&regex, "[0-9]{17}[0-9Xx]", REG_EXTENDED) == 0) {
        if (regexec(&regex, str, 0, NULL, 0) == 0) {
            regfree(&regex);
            return SENSITIVE_ID_CARD;
        }
        regfree(&regex);
    }
    
    return SENSITIVE_NONE;
}

// 过滤敏感信息
char* secure_log_filter_sensitive(const char* input, char* output, size_t output_size) {
    if (!input || !output || output_size == 0) return NULL;
    
    SensitiveType type = secure_log_detect_sensitive(input);
    
    switch (type) {
        case SENSITIVE_PASSWORD:
            strncpy(output, "[FILTERED_PASSWORD]", output_size - 1);
            break;
        case SENSITIVE_TOKEN:
            strncpy(output, "[FILTERED_TOKEN]", output_size - 1);
            break;
        case SENSITIVE_IP:
            strncpy(output, "[FILTERED_IP]", output_size - 1);
            break;
        case SENSITIVE_EMAIL:
            strncpy(output, "[FILTERED_EMAIL]", output_size - 1);
            break;
        case SENSITIVE_PHONE:
            strncpy(output, "[FILTERED_PHONE]", output_size - 1);
            break;
        case SENSITIVE_ID_CARD:
            strncpy(output, "[FILTERED_ID]", output_size - 1);
            break;
        default:
            strncpy(output, input, output_size - 1);
            break;
    }
    
    output[output_size - 1] = '\0';
    return output;
}

// 安全日志记录
void secure_log(LogLevel level, LogType type, const char* file, int line, const char* format, ...) {
    if (level > g_min_level) return;
    
    pthread_mutex_lock(&g_log_mutex);
    
    if (!g_log_file) {
        pthread_mutex_unlock(&g_log_mutex);
        return;
    }
    
    // 获取当前时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[26];
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // 格式化日志消息
    char message[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // 过滤敏感信息
    char filtered_message[4096];
    secure_log_filter_sensitive(message, filtered_message, sizeof(filtered_message));
    
    // 写入日志
    fprintf(g_log_file, "[%s] [%s] [%s] %s:%d %s\n",
            time_str, log_level_str[level], log_type_str[type],
            file ? file : "unknown", line, filtered_message);
    fflush(g_log_file);
    
    pthread_mutex_unlock(&g_log_mutex);
}

// 安全日志记录（带敏感信息标记）
void secure_log_sensitive(LogLevel level, LogType type, const char* file, int line,
                          SensitiveType sensitive_type, const char* format, ...) {
    if (level > g_min_level) return;
    
    pthread_mutex_lock(&g_log_mutex);
    
    if (!g_log_file) {
        pthread_mutex_unlock(&g_log_mutex);
        return;
    }
    
    // 获取当前时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[26];
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // 格式化日志消息
    char message[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // 根据敏感类型过滤
    const char* filtered_str = "[FILTERED]";
    switch (sensitive_type) {
        case SENSITIVE_PASSWORD:
            filtered_str = "[FILTERED_PASSWORD]";
            break;
        case SENSITIVE_TOKEN:
            filtered_str = "[FILTERED_TOKEN]";
            break;
        case SENSITIVE_IP:
            filtered_str = "[FILTERED_IP]";
            break;
        case SENSITIVE_EMAIL:
            filtered_str = "[FILTERED_EMAIL]";
            break;
        case SENSITIVE_PHONE:
            filtered_str = "[FILTERED_PHONE]";
            break;
        case SENSITIVE_ID_CARD:
            filtered_str = "[FILTERED_ID]";
            break;
        default:
            filtered_str = "[FILTERED]";
            break;
    }
    
    // 写入日志
    fprintf(g_log_file, "[%s] [%s] [%s] %s:%d %s\n",
            time_str, log_level_str[level], log_type_str[type],
            file ? file : "unknown", line, filtered_str);
    fflush(g_log_file);
    
    pthread_mutex_unlock(&g_log_mutex);
}

// 加密并写入日志
bool secure_log_write_encrypted(const char* message, size_t len) {
    if (!message || len == 0) return false;
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    
    unsigned char encrypted[4096];
    int encrypted_len = 0;
    int final_len = 0;
    
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, g_aes_key, g_aes_iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_EncryptUpdate(ctx, encrypted, &encrypted_len, (unsigned char*)message, len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_EncryptFinal_ex(ctx, encrypted + encrypted_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    pthread_mutex_lock(&g_log_mutex);
    
    if (g_log_file) {
        fwrite(encrypted, 1, encrypted_len + final_len, g_log_file);
        fflush(g_log_file);
    }
    
    pthread_mutex_unlock(&g_log_mutex);
    
    return true;
}

// 解密日志文件
bool secure_log_decrypt_file(const char* encrypted_path, const char* decrypted_path) {
    if (!encrypted_path || !decrypted_path) return false;
    
    FILE* in_file = fopen(encrypted_path, "rb");
    if (!in_file) return false;
    
    FILE* out_file = fopen(decrypted_path, "w");
    if (!out_file) {
        fclose(in_file);
        return false;
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fclose(in_file);
        fclose(out_file);
        return false;
    }
    
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, g_aes_key, g_aes_iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        fclose(in_file);
        fclose(out_file);
        return false;
    }
    
    unsigned char in_buffer[4096];
    unsigned char out_buffer[4096 + EVP_MAX_BLOCK_LENGTH];
    int in_len = 0;
    int out_len = 0;
    
    while ((in_len = fread(in_buffer, 1, sizeof(in_buffer), in_file)) > 0) {
        if (EVP_DecryptUpdate(ctx, out_buffer, &out_len, in_buffer, in_len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            fclose(in_file);
            fclose(out_file);
            return false;
        }
        fwrite(out_buffer, 1, out_len, out_file);
    }
    
    if (EVP_DecryptFinal_ex(ctx, out_buffer, &out_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        fclose(in_file);
        fclose(out_file);
        return false;
    }
    fwrite(out_buffer, 1, out_len, out_file);
    
    EVP_CIPHER_CTX_free(ctx);
    fclose(in_file);
    fclose(out_file);
    
    return true;
}

// 日志轮转
bool secure_log_rotate(void) {
    pthread_mutex_lock(&g_log_mutex);
    
    if (!g_log_file || strlen(g_log_path) == 0) {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    // 关闭当前日志文件
    fclose(g_log_file);
    g_log_file = NULL;
    
    // 重命名当前日志文件
    char backup_path[256];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(backup_path, sizeof(backup_path), "%s.%Y%m%d_%H%M%S", tm_info);
    
    if (rename(g_log_path, backup_path) != 0) {
        // 重命名失败，尝试重新打开原文件
        g_log_file = fopen(g_log_path, "a");
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    // 创建新的日志文件
    g_log_file = fopen(g_log_path, "a");
    
    pthread_mutex_unlock(&g_log_mutex);
    
    return g_log_file != NULL;
}

// 获取日志文件大小
size_t secure_log_get_file_size(void) {
    pthread_mutex_lock(&g_log_mutex);
    
    if (!g_log_file) {
        pthread_mutex_unlock(&g_log_mutex);
        return 0;
    }
    
    long current_pos = ftell(g_log_file);
    fseek(g_log_file, 0, SEEK_END);
    long size = ftell(g_log_file);
    fseek(g_log_file, current_pos, SEEK_SET);
    
    pthread_mutex_unlock(&g_log_mutex);
    
    return (size_t)size;
}

// 获取日志文件路径
const char* secure_log_get_path(void) {
    return g_log_path;
}
