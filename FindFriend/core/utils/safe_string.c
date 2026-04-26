// FindFriend 安全字符串处理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

FFResult ff_safe_string_copy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
    return FF_OK;
}

FFResult ff_safe_string_concat(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);
    
    if (dest_len + src_len >= dest_size) {
        return FF_ERROR_NO_MEMORY;
    }
    
    strcat(dest, src);
    return FF_OK;
}

FFResult ff_safe_string_printf(char* dest, size_t dest_size, const char* format, ...) {
    if (!dest || !format || dest_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    va_list args;
    va_start(args, format);
    int result = vsnprintf(dest, dest_size, format, args);
    va_end(args);
    
    if (result < 0 || (size_t)result >= dest_size) {
        return FF_ERROR_NO_MEMORY;
    }
    
    return FF_OK;
}

FFResult ff_safe_string_trim(char* str) {
    if (!str) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t len = strlen(str);
    if (len == 0) {
        return FF_OK;
    }
    
    // 移除开头的空白字符
    size_t start = 0;
    while (start < len && isspace((unsigned char)str[start])) {
        start++;
    }
    
    // 移除结尾的空白字符
    size_t end = len - 1;
    while (end > start && isspace((unsigned char)str[end])) {
        end--;
    }
    
    // 复制到新位置
    if (start > 0 || end < len - 1) {
        memmove(str, str + start, end - start + 1);
        str[end - start + 1] = '\0';
    }
    
    return FF_OK;
}

FFResult ff_safe_string_to_lower(char* str) {
    if (!str) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
    
    return FF_OK;
}

FFResult ff_safe_string_to_upper(char* str) {
    if (!str) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
    
    return FF_OK;
}

FFResult ff_safe_string_is_valid(const char* str, size_t max_length) {
    if (!str) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t len = strlen(str);
    if (len == 0 || len > max_length) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 检查是否包含非法字符
    for (size_t i = 0; str[i]; i++) {
        if (!isprint((unsigned char)str[i]) && !isspace((unsigned char)str[i])) {
            return FF_ERROR_INVALID_PARAM;
        }
    }
    
    return FF_OK;
}

FFResult ff_safe_string_validate_username(const char* username) {
    if (!username) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t len = strlen(username);
    if (len < 3 || len > 32) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; username[i]; i++) {
        if (!isalnum((unsigned char)username[i]) && username[i] != '_' && username[i] != '-') {
            return FF_ERROR_INVALID_PARAM;
        }
    }
    
    return FF_OK;
}

FFResult ff_safe_string_validate_password(const char* password) {
    if (!password) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t len = strlen(password);
    if (len < 8) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;
    bool has_special = false;
    
    for (size_t i = 0; password[i]; i++) {
        if (isupper((unsigned char)password[i])) {
            has_upper = true;
        } else if (islower((unsigned char)password[i])) {
            has_lower = true;
        } else if (isdigit((unsigned char)password[i])) {
            has_digit = true;
        } else if (ispunct((unsigned char)password[i])) {
            has_special = true;
        }
    }
    
    if (!has_upper || !has_lower || !has_digit) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    return FF_OK;
}

FFResult ff_safe_string_escape_html(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    dest[0] = '\0';
    size_t dest_pos = 0;
    
    for (size_t i = 0; src[i] && dest_pos < dest_size - 1; i++) {
        switch (src[i]) {
            case '<':
                if (dest_pos + 4 < dest_size) {
                    strcpy(dest + dest_pos, "&lt;");
                    dest_pos += 4;
                } else {
                    return FF_ERROR_NO_MEMORY;
                }
                break;
            case '>':
                if (dest_pos + 4 < dest_size) {
                    strcpy(dest + dest_pos, "&gt;");
                    dest_pos += 4;
                } else {
                    return FF_ERROR_NO_MEMORY;
                }
                break;
            case '&':
                if (dest_pos + 5 < dest_size) {
                    strcpy(dest + dest_pos, "&amp;");
                    dest_pos += 5;
                } else {
                    return FF_ERROR_NO_MEMORY;
                }
                break;
            case '"':
                if (dest_pos + 6 < dest_size) {
                    strcpy(dest + dest_pos, "&quot;");
                    dest_pos += 6;
                } else {
                    return FF_ERROR_NO_MEMORY;
                }
                break;
            case '\'':
                if (dest_pos + 6 < dest_size) {
                    strcpy(dest + dest_pos, "&#39;");
                    dest_pos += 6;
                } else {
                    return FF_ERROR_NO_MEMORY;
                }
                break;
            default:
                dest[dest_pos++] = src[i];
                break;
        }
    }
    
    dest[dest_pos] = '\0';
    return FF_OK;
}

FFResult ff_safe_string_escape_sql(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    dest[0] = '\0';
    size_t dest_pos = 0;
    
    for (size_t i = 0; src[i] && dest_pos < dest_size - 1; i++) {
        if (src[i] == '\'') {
            if (dest_pos + 2 < dest_size) {
                dest[dest_pos++] = '\'';
                dest[dest_pos++] = '\'';
            } else {
                return FF_ERROR_NO_MEMORY;
            }
        } else {
            dest[dest_pos++] = src[i];
        }
    }
    
    dest[dest_pos] = '\0';
    return FF_OK;
}

FFResult ff_safe_string_generate_random(char* dest, size_t length) {
    if (!dest || length == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_len = strlen(charset);
    
    for (size_t i = 0; i < length - 1; i++) {
        int index = rand() % charset_len;
        dest[i] = charset[index];
    }
    
    dest[length - 1] = '\0';
    return FF_OK;
}

size_t ff_safe_string_hash(const char* str) {
    if (!str) {
        return 0;
    }
    
    size_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}
