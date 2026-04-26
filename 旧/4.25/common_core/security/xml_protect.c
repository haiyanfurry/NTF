#include "xml_protect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 允许的XML标签白名单
static const char* allowed_tags[] = {
    "user",
    "id",
    "username",
    "password",
    "email",
    "name",
    "age",
    "address",
    "phone",
    "message",
    "content",
    "title",
    "description",
    "url",
    "image",
    "video",
    "audio",
    "file",
    "exhibition",
    "ticket",
    "friend",
    "location",
    "latitude",
    "longitude",
    "timestamp",
    "status",
    "type",
    NULL
};

// 特殊字符映射
static struct {
    char* special;   // 特殊字符
    char* escaped;   // 转义后的字符
} special_chars[] = {
    {"&", "&amp;"},
    {"<", "&lt;"},
    {">", "&gt;"},
    {"\"", "&quot;"},
    {"'", "&#39;"},
    {"\"", "&apos;"},
    {NULL, NULL}
};

// 初始化XML保护模块
bool xml_protect_init(void) {
    printf("XML protection module initialized\n");
    return true;
}

// 清理XML保护模块
void xml_protect_cleanup(void) {
    printf("XML protection module cleaned up\n");
}

// 检查XML标签是否在白名单中
bool xml_is_tag_allowed(const char* tag) {
    if (!tag) {
        return false;
    }
    
    for (int i = 0; allowed_tags[i]; i++) {
        if (strcmp(tag, allowed_tags[i]) == 0) {
            return true;
        }
    }
    
    return false;
}

// 转义XML特殊字符
char* xml_escape_special_chars(const char* input) {
    if (!input) {
        return NULL;
    }
    
    size_t input_len = strlen(input);
    size_t output_len = input_len * 5; // 最大可能的长度（每个字符都被转义）
    char* output = (char*)malloc(output_len + 1);
    if (!output) {
        return NULL;
    }
    
    output[0] = '\0';
    size_t pos = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        bool escaped = false;
        for (int j = 0; special_chars[j].special; j++) {
            if (strncmp(&input[i], special_chars[j].special, strlen(special_chars[j].special)) == 0) {
                strcat(output, special_chars[j].escaped);
                pos += strlen(special_chars[j].escaped);
                i += strlen(special_chars[j].special) - 1;
                escaped = true;
                break;
            }
        }
        if (!escaped) {
            output[pos++] = input[i];
            output[pos] = '\0';
        }
    }
    
    return output;
}

// 过滤XML数据
char* xml_filter(const char* xml_data) {
    if (!xml_data) {
        return NULL;
    }
    
    // 这里可以实现更复杂的XML过滤逻辑
    // 例如移除不允许的标签，转义特殊字符等
    char* escaped_data = xml_escape_special_chars(xml_data);
    return escaped_data;
}

// 检测XML注入攻击
bool xml_detect_injection(const char* xml_data) {
    if (!xml_data) {
        return false;
    }
    
    // 检测常见的XML注入攻击模式
    const char* patterns[] = {
        "<![CDATA[",
        "<!--",
        "-->\n",
        "]]>",
        "<!DOCTYPE",
        "<!ENTITY",
        "SYSTEM",
        "PUBLIC",
        "../",
        "../../",
        "/etc/passwd",
        "/etc/shadow",
        NULL
    };
    
    for (int i = 0; patterns[i]; i++) {
        if (strstr(xml_data, patterns[i])) {
            return true;
        }
    }
    
    return false;
}

// 验证XML格式是否安全
bool xml_validate_security(const char* xml_data) {
    if (!xml_data) {
        return false;
    }
    
    // 检测XML注入攻击
    if (xml_detect_injection(xml_data)) {
        return false;
    }
    
    // 这里可以添加更多的XML安全验证逻辑
    // 例如验证标签嵌套是否正确，属性值是否安全等
    
    return true;
}
