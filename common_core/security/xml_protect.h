#ifndef XML_PROTECT_H
#define XML_PROTECT_H

#include <stdbool.h>
#include <stdint.h>

// 初始化XML保护模块
bool xml_protect_init(void);

// 清理XML保护模块
void xml_protect_cleanup(void);

// 过滤XML数据
char* xml_filter(const char* xml_data);

// 检查XML标签是否在白名单中
bool xml_is_tag_allowed(const char* tag);

// 转义XML特殊字符
char* xml_escape_special_chars(const char* input);

// 验证XML格式是否安全
bool xml_validate_security(const char* xml_data);

// 检测XML注入攻击
bool xml_detect_injection(const char* xml_data);

#endif // XML_PROTECT_H
