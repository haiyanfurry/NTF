// ====================================================================
// FindFriend C 代码质量优化：安全辅助函数头文件
// ====================================================================

#ifndef FF_SAFE_UTILS_H
#define FF_SAFE_UTILS_H

#include "rust_modules/c_bindings/ff_rust_bindings.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ====================================================================
// 日志系统
// ====================================================================

void ff_log_set_level(int level);
void ff_log(int level, const char* fmt, ...);

#define FF_LOG(level, fmt, ...) ff_log(level, fmt, ##__VA_ARGS__)
#define FF_DEBUG(fmt, ...) ff_log(FF_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define FF_INFO(fmt, ...) ff_log(FF_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define FF_WARN(fmt, ...) ff_log(FF_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define FF_ERROR(fmt, ...) ff_log(FF_LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

// ====================================================================
// 安全字符串操作
// ====================================================================

size_t ff_safe_strncpy(char* dest, const char* src, size_t dest_size);
size_t ff_safe_strncat(char* dest, const char* src, size_t dest_size);
int ff_safe_snprintf(char* buffer, size_t buffer_size, const char* fmt, ...);

#define STRCPY_SAFE(dest, src, size) ff_safe_strncpy(dest, src, size)
#define STRCAT_SAFE(dest, src, size) ff_safe_strncat(dest, src, size)
#define SNPRINTF_SAFE(buffer, size, fmt, ...) ff_safe_snprintf(buffer, size, fmt, ##__VA_ARGS__)

// ====================================================================
// 内存安全操作
// ====================================================================

void* ff_safe_malloc(size_t size, const char* filename, int line);
void ff_safe_free(void** ptr);

#define FF_MALLOC(size) ff_safe_malloc(size, __FILE__, __LINE__)
#define FF_FREE(ptr) ff_safe_free((void**)&(ptr))

void ff_check_memory_leaks(void);

// ====================================================================
// 数组边界检查
// ====================================================================

bool ff_bounds_check(size_t index, size_t array_size, const char* array_name, const char* file, int line);
bool ff_buffer_overflow_check(size_t write_len, size_t buffer_size, const char* buffer_name, const char* file, int line);

#define FF_BOUNDS_CHECK(index, array_size) ff_bounds_check(index, array_size, #array_size, __FILE__, __LINE__)
#define FF_BUFFER_CHECK(write_len, buffer_size) ff_buffer_overflow_check(write_len, buffer_size, #buffer_size, __FILE__, __LINE__)

// ====================================================================
// 并发安全辅助宏
// ====================================================================

#define FF_MUTEX_LOCK(mutex, name) do { \
    int result = pthread_mutex_lock(mutex); \
    if (result != 0) { \
        FF_ERROR("Failed to lock mutex %s: %d", name, result); \
    } \
} while(0)

#define FF_MUTEX_UNLOCK(mutex, name) do { \
    int result = pthread_mutex_unlock(mutex); \
    if (result != 0) { \
        FF_ERROR("Failed to unlock mutex %s: %d", name, result); \
    } \
} while(0)

#ifdef __cplusplus
}
#endif

#endif // FF_SAFE_UTILS_H
