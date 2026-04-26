// ====================================================================
// FindFriend C 代码质量优化：安全辅助函数
// - 安全字符串操作
// - 内存安全检查
// - 统一日志系统
// ====================================================================

#ifdef FF_USE_RUST_MODULES
#include "rust_modules/c_bindings/ff_rust_bindings.h"
#else
// 模拟 Rust 绑定的宏定义
#define FF_LOG_LEVEL_DEBUG 0
#define FF_LOG_LEVEL_INFO 1
#define FF_LOG_LEVEL_WARN 2
#define FF_LOG_LEVEL_ERROR 3
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

// ====================================================================
// 日志系统实现
// ====================================================================

static int log_level = FF_LOG_LEVEL_INFO;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void ff_log_set_level(int level) {
    log_level = level;
}

static const char* level_str(int level) {
    switch(level) {
        case FF_LOG_LEVEL_DEBUG: return "[DEBUG]";
        case FF_LOG_LEVEL_INFO: return "[INFO]";
        case FF_LOG_LEVEL_WARN: return "[WARN]";
        case FF_LOG_LEVEL_ERROR: return "[ERROR]";
        default: return "[UNKNOWN]";
    }
}

void ff_log(int level, const char* fmt, ...) {
    if (level < log_level) {
        return;
    }

    pthread_mutex_lock(&log_mutex);
    
    time_t now = time(NULL);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(stderr, "%s %s ", time_str, level_str(level));
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    fflush(stderr);
    
    pthread_mutex_unlock(&log_mutex);
}

// ====================================================================
// 安全字符串操作
// ====================================================================

size_t ff_safe_strncpy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        ff_log(FF_LOG_LEVEL_ERROR, "ff_safe_strncpy: invalid parameters");
        return 0;
    }

    size_t src_len = strlen(src);
    
    if (src_len >= dest_size) {
        ff_log(FF_LOG_LEVEL_WARN, "ff_safe_strncpy: string truncated (src=%zu, dest=%zu)", src_len, dest_size);
        memcpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
        return dest_size - 1;
    } else {
        memcpy(dest, src, src_len + 1);
        return src_len;
    }
}

size_t ff_safe_strncat(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        ff_log(FF_LOG_LEVEL_ERROR, "ff_safe_strncat: invalid parameters");
        return 0;
    }

    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size) {
        ff_log(FF_LOG_LEVEL_ERROR, "ff_safe_strncat: destination already full");
        return dest_len;
    }

    size_t remaining = dest_size - dest_len - 1;
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < remaining) ? src_len : remaining;

    if (copy_len > 0) {
        memcpy(dest + dest_len, src, copy_len);
        dest[dest_len + copy_len] = '\0';
    }

    if (src_len > remaining) {
        ff_log(FF_LOG_LEVEL_WARN, "ff_safe_strncat: string truncated");
    }

    return dest_len + copy_len;
}

int ff_safe_snprintf(char* buffer, size_t buffer_size, const char* fmt, ...) {
    if (!buffer || buffer_size == 0 || !fmt) {
        ff_log(FF_LOG_LEVEL_ERROR, "ff_safe_snprintf: invalid parameters");
        return -1;
    }

    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(buffer, buffer_size, fmt, args);
    va_end(args);

    if (result < 0) {
        ff_log(FF_LOG_LEVEL_ERROR, "ff_safe_snprintf: encoding error");
        return -1;
    }

    if ((size_t)result >= buffer_size) {
        ff_log(FF_LOG_LEVEL_WARN, "ff_safe_snprintf: output truncated (would need %d bytes)", result);
    }

    return result;
}

// ====================================================================
// 内存安全操作
// ====================================================================

typedef struct AllocationHeader {
    size_t size;
    const char* file;
    int line;
    struct AllocationHeader* prev;
    struct AllocationHeader* next;
} AllocationHeader;

static AllocationHeader* alloc_list = NULL;
static pthread_mutex_t alloc_mutex = PTHREAD_MUTEX_INITIALIZER;

void* ff_safe_malloc(size_t size, const char* filename, int line) {
    if (size == 0) {
        ff_log(FF_LOG_LEVEL_WARN, "ff_safe_malloc: zero size requested");
        size = 1;
    }

    void* ptr = malloc(size + sizeof(AllocationHeader));
    if (!ptr) {
        ff_log(FF_LOG_LEVEL_ERROR, "ff_safe_malloc: out of memory (requested %zu bytes)", size);
        return NULL;
    }

    AllocationHeader* header = (AllocationHeader*)ptr;
    header->size = size;
    header->file = filename;
    header->line = line;

    pthread_mutex_lock(&alloc_mutex);
    header->prev = NULL;
    header->next = alloc_list;
    if (alloc_list) {
        alloc_list->prev = header;
    }
    alloc_list = header;
    pthread_mutex_unlock(&alloc_mutex);

    ff_log(FF_LOG_LEVEL_DEBUG, "Allocated %zu bytes at %p (%s:%d)", size, ((char*)ptr + sizeof(AllocationHeader)), filename, line);

    return (char*)ptr + sizeof(AllocationHeader);
}

void ff_safe_free(void** ptr) {
    if (!ptr || !*ptr) {
        ff_log(FF_LOG_LEVEL_DEBUG, "ff_safe_free: null pointer (nothing to free)");
        return;
    }

    AllocationHeader* header = (AllocationHeader*)((char*)*ptr - sizeof(AllocationHeader));
    
    ff_log(FF_LOG_LEVEL_DEBUG, "Freeing %zu bytes at %p (%s:%d)", header->size, *ptr, header->file, header->line);

    pthread_mutex_lock(&alloc_mutex);
    if (header->prev) {
        header->prev->next = header->next;
    }
    if (header->next) {
        header->next->prev = header->prev;
    }
    if (alloc_list == header) {
        alloc_list = header->next;
    }
    pthread_mutex_unlock(&alloc_mutex);

    memset(header, 0x55, sizeof(AllocationHeader) + header->size);
    free(header);
    *ptr = NULL;
}

void ff_check_memory_leaks(void) {
    pthread_mutex_lock(&alloc_mutex);

    if (!alloc_list) {
        ff_log(FF_LOG_LEVEL_INFO, "ff_check_memory_leaks: no leaks detected!");
        pthread_mutex_unlock(&alloc_mutex);
        return;
    }

    ff_log(FF_LOG_LEVEL_ERROR, "ff_check_memory_leaks: MEMORY LEAKS DETECTED!");
    
    int leak_count = 0;
    size_t total_leaked = 0;
    AllocationHeader* current = alloc_list;
    while (current) {
        ff_log(FF_LOG_LEVEL_ERROR, "  [LEAK] %zu bytes at %p (%s:%d)", current->size, (char*)current + sizeof(AllocationHeader), current->file, current->line);
        leak_count++;
        total_leaked += current->size;
        current = current->next;
    }

    ff_log(FF_LOG_LEVEL_ERROR, "Total: %d leaks, %zu bytes leaked", leak_count, total_leaked);
    pthread_mutex_unlock(&alloc_mutex);
}

// ====================================================================
// 数组边界检查辅助
// ====================================================================

bool ff_bounds_check(size_t index, size_t array_size, const char* array_name, const char* file, int line) {
    if (index >= array_size) {
        ff_log(FF_LOG_LEVEL_ERROR, "Array out of bounds: %s[%zu] (size=%zu) at %s:%d", array_name, index, array_size, file, line);
        return false;
    }
    return true;
}

bool ff_buffer_overflow_check(size_t write_len, size_t buffer_size, const char* buffer_name, const char* file, int line) {
    if (write_len > buffer_size) {
        ff_log(FF_LOG_LEVEL_ERROR, "Buffer overflow risk: %s (writing %zu to %zu) at %s:%d", buffer_name, write_len, buffer_size, file, line);
        return false;
    }
    return true;
}

#define FF_BOUNDS_CHECK(index, array_size) ff_bounds_check(index, array_size, #array_size, __FILE__, __LINE__)
#define FF_BUFFER_CHECK(write_len, buffer_size) ff_buffer_overflow_check(write_len, buffer_size, #buffer_size, __FILE__, __LINE__)
