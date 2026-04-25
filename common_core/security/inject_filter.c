#include "inject_filter.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// 最大输入长度限制
#define MAX_INPUT_LENGTH 1024

// 安全的字符串复制函数
char* safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return NULL;
    }
    
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        src_len = dest_size - 1;
    }
    
    strncpy(dest, src, src_len);
    dest[src_len] = '\0';
    return dest;
}

// 过滤 SQL 注入
int filter_sql_injection(const char* input) {
    if (input == NULL) {
        return 0;
    }
    
    // 常见的 SQL 注入关键字
    const char* sql_keywords[] = {
        "DROP", "DELETE", "INSERT", "UPDATE", "SELECT", "UNION",
        "OR", "AND", "NOT", "FROM", "WHERE", "JOIN",
        "EXEC", "EXECUTE", "XP_CMDSHELL", ";--", "#", "--",
        NULL
    };
    
    for (int i = 0; sql_keywords[i] != NULL; i++) {
        if (strstr(input, sql_keywords[i]) != NULL) {
            return 1; // 检测到 SQL 注入
        }
    }
    
    return 0; // 安全
}

// 过滤 XSS 攻击
int filter_xss(const char* input) {
    if (input == NULL) {
        return 0;
    }
    
    // 常见的 XSS 关键字
    const char* xss_keywords[] = {
        "<script", "</script>", "<iframe", "</iframe>",
        "javascript:", "onload=", "onerror=", "onclick=",
        "<object", "</object>", "<embed", "</embed>",
        NULL
    };
    
    for (int i = 0; xss_keywords[i] != NULL; i++) {
        if (strstr(input, xss_keywords[i]) != NULL) {
            return 1; // 检测到 XSS 攻击
        }
    }
    
    return 0; // 安全
}

// 过滤命令注入
int filter_command_injection(const char* input) {
    if (input == NULL) {
        return 0;
    }
    
    // 常见的命令注入字符
    const char* cmd_chars[] = {
        ";", "||", "&&", "|" , "`", "$(",
        NULL
    };
    
    for (int i = 0; cmd_chars[i] != NULL; i++) {
        if (strstr(input, cmd_chars[i]) != NULL) {
            return 1; // 检测到命令注入
        }
    }
    
    return 0; // 安全
}

// 检查输入长度
int check_input_length(const char* input, size_t max_length) {
    if (input == NULL) {
        return 0;
    }
    
    return strlen(input) <= max_length;
}

// 过滤 XML 畸形包和 XXE
int filter_xml_injection(const char* input) {
    if (input == NULL) {
        return 0;
    }
    
    // 检测 XXE 攻击
    const char* xxe_patterns[] = {
        "<!DOCTYPE", "<!ENTITY", "SYSTEM", "PUBLIC",
        NULL
    };
    
    for (int i = 0; xxe_patterns[i] != NULL; i++) {
        if (strstr(input, xxe_patterns[i]) != NULL) {
            return 1; // 检测到 XML 注入
        }
    }
    
    return 0; // 安全
}

// 综合输入验证
int validate_input(const char* input, size_t max_length) {
    if (input == NULL) {
        return 0;
    }
    
    // 检查长度
    if (!check_input_length(input, max_length)) {
        return 1;
    }
    
    // 检查 SQL 注入
    if (filter_sql_injection(input)) {
        return 1;
    }
    
    // 检查 XSS 攻击
    if (filter_xss(input)) {
        return 1;
    }
    
    // 检查命令注入
    if (filter_command_injection(input)) {
        return 1;
    }
    
    // 检查 XML 注入
    if (filter_xml_injection(input)) {
        return 1;
    }
    
    return 0; // 安全
}

// 初始化注入过滤模块
bool inject_filter_init(void) {
    // 注入过滤模块不需要特殊初始化
    return true;
}

// 清理注入过滤模块
void inject_filter_cleanup(void) {
    // 无资源需要释放
}