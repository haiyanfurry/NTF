#define _GNU_SOURCE

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

// 字符串工具函数
char* utils_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* dup = (char*)malloc(len);
    if (dup) {
        memcpy(dup, str, len);
    }
    return dup;
}

char* utils_strndup(const char* str, size_t n) {
    if (!str) return NULL;
    size_t len = strlen(str);
    if (len > n) len = n;
    char* dup = (char*)malloc(len + 1);
    if (dup) {
        memcpy(dup, str, len);
        dup[len] = '\0';
    }
    return dup;
}

int utils_strcasecmp(const char* a, const char* b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    while (*a && *b) {
        if (tolower(*a) != tolower(*b)) {
            return tolower(*a) - tolower(*b);
        }
        a++;
        b++;
    }
    
    return tolower(*a) - tolower(*b);
}

char* utils_strtrim(char* str) {
    if (!str) return NULL;
    
    char* start = str;
    while (*start && isspace(*start)) start++;
    
    if (!*start) {
        *str = '\0';
        return str;
    }
    
    char* end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    *(end + 1) = '\0';
    
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
    
    return str;
}

char** utils_strsplit(const char* str, char delimiter, size_t* count) {
    if (!str || !count) return NULL;
    
    size_t cnt = 1;
    const char* p = str;
    while (*p) {
        if (*p == delimiter) cnt++;
        p++;
    }
    
    char** parts = (char**)malloc(sizeof(char*) * (cnt + 1));
    if (!parts) return NULL;
    
    size_t idx = 0;
    const char* start = str;
    p = str;
    
    while (true) {
        if (*p == delimiter || *p == '\0') {
            size_t len = p - start;
            parts[idx] = (char*)malloc(len + 1);
            if (parts[idx]) {
                memcpy(parts[idx], start, len);
                parts[idx][len] = '\0';
            }
            idx++;
            if (*p == '\0') break;
            start = p + 1;
        }
        p++;
    }
    
    *count = idx;
    parts[idx] = NULL;
    
    return parts;
}

void utils_strsplit_free(char** parts, size_t count) {
    if (!parts) return;
    for (size_t i = 0; i < count; i++) {
        if (parts[i]) free(parts[i]);
    }
    free(parts);
}

char* utils_strjoin(const char** parts, size_t count, const char* delimiter) {
    if (!parts || count == 0) return NULL;
    
    size_t delim_len = delimiter ? strlen(delimiter) : 0;
    size_t total_len = 0;
    for (size_t i = 0; i < count; i++) {
        total_len += strlen(parts[i]);
        if (i < count - 1) total_len += delim_len;
    }
    
    char* result = (char*)malloc(total_len + 1);
    if (!result) return NULL;
    
    char* p = result;
    for (size_t i = 0; i < count; i++) {
        size_t len = strlen(parts[i]);
        memcpy(p, parts[i], len);
        p += len;
        if (i < count - 1 && delimiter) {
            memcpy(p, delimiter, delim_len);
            p += delim_len;
        }
    }
    *p = '\0';
    
    return result;
}

bool utils_strstartswith(const char* str, const char* prefix) {
    if (!str || !prefix) return false;
    size_t prefix_len = strlen(prefix);
    return strlen(str) >= prefix_len && strncmp(str, prefix, prefix_len) == 0;
}

bool utils_strendswith(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return str_len >= suffix_len && strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

// 内存工具函数
void* utils_malloc(size_t size) {
    return malloc(size);
}

void* utils_calloc(size_t count, size_t size) {
    return calloc(count, size);
}

void* utils_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

void utils_free(void* ptr) {
    free(ptr);
}

void* utils_memdup(const void* src, size_t size) {
    if (!src || size == 0) return NULL;
    void* dup = malloc(size);
    if (dup) {
        memcpy(dup, src, size);
    }
    return dup;
}

// 文件工具函数
bool utils_file_exists(const char* path) {
    if (!path) return false;
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

bool utils_dir_exists(const char* path) {
    if (!path) return false;
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool utils_mkdir(const char* path) {
    if (!path) return false;
    return mkdir(path, 0755) == 0;
}

bool utils_remove(const char* path) {
    if (!path) return false;
    return unlink(path) == 0;
}

size_t utils_file_size(const char* path) {
    if (!path) return 0;
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return st.st_size;
}

char* utils_read_file(const char* path, size_t* size) {
    if (!path) return NULL;
    
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;
    
    fseek(file, 0, SEEK_END);
    size_t fsize = ftell(file);
    rewind(file);
    
    char* data = (char*)malloc(fsize + 1);
    if (!data) {
        fclose(file);
        return NULL;
    }
    
    size_t read_size = fread(data, 1, fsize, file);
    fclose(file);
    
    if (read_size != fsize) {
        free(data);
        return NULL;
    }
    
    data[fsize] = '\0';
    
    if (size) *size = fsize;
    return data;
}

bool utils_write_file(const char* path, const void* data, size_t size) {
    if (!path || !data) return false;
    
    FILE* file = fopen(path, "wb");
    if (!file) return false;
    
    size_t written = fwrite(data, 1, size, file);
    fclose(file);
    
    return written == size;
}

char* utils_get_home_dir(void) {
    const char* home = getenv("HOME");
    return home ? utils_strdup(home) : NULL;
}

char* utils_get_temp_dir(void) {
    const char* temp = getenv("TMPDIR");
    if (!temp) temp = "/tmp";
    return utils_strdup(temp);
}

// 时间工具函数
int64_t utils_get_timestamp_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int64_t utils_get_timestamp_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

void utils_sleep_ms(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

char* utils_timestamp_to_string(int64_t timestamp) {
    time_t t = (time_t)(timestamp / 1000);
    struct tm* tm = localtime(&t);
    if (!tm) return NULL;
    
    char* buf = (char*)malloc(32);
    if (!buf) return NULL;
    
    strftime(buf, 32, "%Y-%m-%d %H:%M:%S", tm);
    return buf;
}

char* utils_get_current_time_string(void) {
    return utils_timestamp_to_string(utils_get_timestamp_ms());
}

// 随机数工具函数
uint32_t utils_random_uint32(void) {
    static bool initialized = false;
    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = true;
    }
    return (uint32_t)rand();
}

int32_t utils_random_int32(void) {
    return (int32_t)utils_random_uint32();
}

double utils_random_double(void) {
    return (double)utils_random_uint32() / (double)UINT32_MAX;
}

void utils_random_bytes(uint8_t* buffer, size_t size) {
    if (!buffer) return;
    
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)(utils_random_uint32() & 0xFF);
    }
}

char* utils_generate_uuid(void) {
    uint8_t bytes[16];
    utils_random_bytes(bytes, 16);
    
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;
    
    char* uuid = (char*)malloc(37);
    if (!uuid) return NULL;
    
    snprintf(uuid, 37, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             bytes[0], bytes[1], bytes[2], bytes[3],
             bytes[4], bytes[5], bytes[6], bytes[7],
             bytes[8], bytes[9], bytes[10], bytes[11],
             bytes[12], bytes[13], bytes[14], bytes[15]);
    
    return uuid;
}

// 编码工具函数
char* utils_base64_encode(const uint8_t* data, size_t size, size_t* output_size) {
    static const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    if (!data || size == 0) return NULL;
    
    size_t out_size = ((size + 2) / 3) * 4 + 1;
    char* out = (char*)malloc(out_size);
    if (!out) return NULL;
    
    size_t i = 0, j = 0;
    while (i < size) {
        uint32_t a = (i < size) ? data[i++] : 0;
        uint32_t b = (i < size) ? data[i++] : 0;
        uint32_t c = (i < size) ? data[i++] : 0;
        uint32_t triple = (a << 16) | (b << 8) | c;
        
        out[j++] = chars[(triple >> 18) & 0x3F];
        out[j++] = chars[(triple >> 12) & 0x3F];
        out[j++] = (i > size + 1) ? '=' : chars[(triple >> 6) & 0x3F];
        out[j++] = (i > size) ? '=' : chars[triple & 0x3F];
    }
    
    out[j] = '\0';
    if (output_size) *output_size = j;
    
    return out;
}

uint8_t* utils_base64_decode(const char* str, size_t* output_size) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    if (len == 0 || len % 4 != 0) return NULL;
    
    size_t padding = 0;
    if (str[len - 1] == '=') padding++;
    if (str[len - 2] == '=') padding++;
    
    size_t out_size = (len * 3 / 4) - padding;
    uint8_t* out = (uint8_t*)malloc(out_size);
    if (!out) return NULL;
    
    uint8_t vals[256];
    memset(vals, 0xFF, 256);
    for (int i = 0; i < 64; i++) {
        vals[(uint8_t)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = (uint8_t)i;
    }
    vals[(uint8_t)'='] = 0;
    
    size_t i = 0, j = 0;
    while (i < len) {
        uint32_t a = vals[(uint8_t)str[i++]];
        uint32_t b = vals[(uint8_t)str[i++]];
        uint32_t c = vals[(uint8_t)str[i++]];
        uint32_t d = vals[(uint8_t)str[i++]];
        uint32_t triple = (a << 18) | (b << 12) | (c << 6) | d;
        
        if (j < out_size) out[j++] = (triple >> 16) & 0xFF;
        if (j < out_size) out[j++] = (triple >> 8) & 0xFF;
        if (j < out_size) out[j++] = triple & 0xFF;
    }
    
    if (output_size) *output_size = out_size;
    return out;
}

char* utils_url_encode(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    size_t out_size = len * 3 + 1;
    char* out = (char*)malloc(out_size);
    if (!out) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out[j++] = c;
        } else {
            snprintf(out + j, 4, "%%%02X", (unsigned char)c);
            j += 3;
        }
    }
    out[j] = '\0';
    
    return out;
}

char* utils_url_decode(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* out = (char*)malloc(len + 1);
    if (!out) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '%' && i + 2 < len) {
            char hex[3] = {str[i + 1], str[i + 2], '\0'};
            out[j++] = (char)strtol(hex, NULL, 16);
            i += 2;
        } else {
            out[j++] = str[i];
        }
    }
    out[j] = '\0';
    
    return out;
}

// 数学工具函数
int utils_clamp_int(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

double utils_clamp_double(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int utils_max_int(int a, int b) {
    return (a > b) ? a : b;
}

int utils_min_int(int a, int b) {
    return (a < b) ? a : b;
}

double utils_max_double(double a, double b) {
    return (a > b) ? a : b;
}

double utils_min_double(double a, double b) {
    return (a < b) ? a : b;
}

// 版本工具函数
bool utils_version_compare(const char* v1, const char* v2, int* result) {
    if (!v1 || !v2 || !result) return false;
    
    int v1_major, v1_minor, v1_patch;
    int v2_major, v2_minor, v2_patch;
    
    if (sscanf(v1, "%d.%d.%d", &v1_major, &v1_minor, &v1_patch) != 3) {
        if (sscanf(v1, "%d.%d", &v1_major, &v1_minor) != 2) {
            if (sscanf(v1, "%d", &v1_major) != 1) {
                return false;
            }
            v1_minor = 0;
            v1_patch = 0;
        } else {
            v1_patch = 0;
        }
    }
    
    if (sscanf(v2, "%d.%d.%d", &v2_major, &v2_minor, &v2_patch) != 3) {
        if (sscanf(v2, "%d.%d", &v2_major, &v2_minor) != 2) {
            if (sscanf(v2, "%d", &v2_major) != 1) {
                return false;
            }
            v2_minor = 0;
            v2_patch = 0;
        } else {
            v2_patch = 0;
        }
    }
    
    if (v1_major != v2_major) {
        *result = (v1_major > v2_major) ? 1 : -1;
    } else if (v1_minor != v2_minor) {
        *result = (v1_minor > v2_minor) ? 1 : -1;
    } else if (v1_patch != v2_patch) {
        *result = (v1_patch > v2_patch) ? 1 : -1;
    } else {
        *result = 0;
    }
    
    return true;
}

char* utils_version_format(int major, int minor, int patch) {
    char* buf = (char*)malloc(32);
    if (!buf) return NULL;
    snprintf(buf, 32, "%d.%d.%d", major, minor, patch);
    return buf;
}
