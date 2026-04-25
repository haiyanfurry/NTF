#ifndef INJECT_FILTER_H
#define INJECT_FILTER_H

#include <stddef.h>

// 安全的字符串复制函数
char* safe_strcpy(char* dest, const char* src, size_t dest_size);

// 过滤 SQL 注入
int filter_sql_injection(const char* input);

// 过滤 XSS 攻击
int filter_xss(const char* input);

// 过滤命令注入
int filter_command_injection(const char* input);

// 检查输入长度
int check_input_length(const char* input, size_t max_length);

// 过滤 XML 畸形包和 XXE
int filter_xml_injection(const char* input);

// 综合输入验证
int validate_input(const char* input, size_t max_length);

#endif // INJECT_FILTER_H