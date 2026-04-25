#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 字符串工具函数
char* utils_strdup(const char* str);
char* utils_strndup(const char* str, size_t n);
int utils_strcasecmp(const char* a, const char* b);
char* utils_strtrim(char* str);
char** utils_strsplit(const char* str, char delimiter, size_t* count);
void utils_strsplit_free(char** parts, size_t count);
char* utils_strjoin(const char** parts, size_t count, const char* delimiter);
bool utils_strstartswith(const char* str, const char* prefix);
bool utils_strendswith(const char* str, const char* suffix);

// 内存工具函数
void* utils_malloc(size_t size);
void* utils_calloc(size_t count, size_t size);
void* utils_realloc(void* ptr, size_t size);
void utils_free(void* ptr);
void* utils_memdup(const void* src, size_t size);

// 文件工具函数
bool utils_file_exists(const char* path);
bool utils_dir_exists(const char* path);
bool utils_mkdir(const char* path);
bool utils_remove(const char* path);
size_t utils_file_size(const char* path);
char* utils_read_file(const char* path, size_t* size);
bool utils_write_file(const char* path, const void* data, size_t size);
char* utils_get_home_dir(void);
char* utils_get_temp_dir(void);

// 时间工具函数
int64_t utils_get_timestamp_ms(void);
int64_t utils_get_timestamp_us(void);
void utils_sleep_ms(uint32_t ms);
char* utils_timestamp_to_string(int64_t timestamp);
char* utils_get_current_time_string(void);

// 随机数工具函数
uint32_t utils_random_uint32(void);
int32_t utils_random_int32(void);
double utils_random_double(void);
void utils_random_bytes(uint8_t* buffer, size_t size);
char* utils_generate_uuid(void);

// 编码工具函数
char* utils_base64_encode(const uint8_t* data, size_t size, size_t* output_size);
uint8_t* utils_base64_decode(const char* str, size_t* output_size);
char* utils_url_encode(const char* str);
char* utils_url_decode(const char* str);

// 数学工具函数
int utils_clamp_int(int value, int min, int max);
double utils_clamp_double(double value, double min, double max);
int utils_max_int(int a, int b);
int utils_min_int(int a, int b);
double utils_max_double(double a, double b);
double utils_min_double(double a, double b);

// 版本工具函数
bool utils_version_compare(const char* v1, const char* v2, int* result);
char* utils_version_format(int major, int minor, int patch);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
