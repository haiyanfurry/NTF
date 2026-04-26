#include "safe_memory.h"
#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG_MEMORY
#define MAX_TRACKED_ALLOCS 10000

static struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    bool active;
} g_allocations[MAX_TRACKED_ALLOCS];

static int g_alloc_count = 0;
static pthread_mutex_t g_mem_mutex = PTHREAD_MUTEX_INITIALIZER;

static void track_allocation(void* ptr, size_t size, const char* file, int line) {
    if (!ptr) return;
    
    pthread_mutex_lock(&g_mem_mutex);
    for (int i = 0; i < MAX_TRACKED_ALLOCS; i++) {
        if (!g_allocations[i].active) {
            g_allocations[i].ptr = ptr;
            g_allocations[i].size = size;
            g_allocations[i].file = file;
            g_allocations[i].line = line;
            g_allocations[i].active = true;
            g_alloc_count++;
            pthread_mutex_unlock(&g_mem_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&g_mem_mutex);
    fprintf(stderr, "[MEMORY] Warning: Allocation tracking table full\n");
}

static void untrack_allocation(void* ptr) {
    if (!ptr) return;
    
    pthread_mutex_lock(&g_mem_mutex);
    for (int i = 0; i < MAX_TRACKED_ALLOCS; i++) {
        if (g_allocations[i].active && g_allocations[i].ptr == ptr) {
            g_allocations[i].active = false;
            g_alloc_count--;
            pthread_mutex_unlock(&g_mem_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&g_mem_mutex);
}
#endif

void* safe_malloc(size_t size, const char* file, int line) {
    if (size == 0) {
        fprintf(stderr, "[MEMORY] Warning: Zero-size allocation at %s:%d\n", file, line);
        return NULL;
    }
    
    if (size > 1024 * 1024 * 1024) { // 1GB limit
        fprintf(stderr, "[MEMORY] Error: Allocation size too large (%zu) at %s:%d\n", size, file, line);
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "[MEMORY] Error: Failed to allocate %zu bytes at %s:%d\n", size, file, line);
        return NULL;
    }
    
    // 清零内存
    memset(ptr, 0, size);
    
#ifdef DEBUG_MEMORY
    track_allocation(ptr, size, file, line);
    printf("[MEMORY] Allocated %zu bytes at %p (%s:%d)\n", size, ptr, file, line);
#endif
    
    return ptr;
}

void* safe_calloc(size_t nmemb, size_t size, const char* file, int line) {
    if (nmemb == 0 || size == 0) {
        fprintf(stderr, "[MEMORY] Warning: Zero-size calloc at %s:%d\n", file, line);
        return NULL;
    }
    
    // 检查溢出
    if (nmemb > SIZE_MAX / size) {
        fprintf(stderr, "[MEMORY] Error: Calloc overflow at %s:%d\n", file, line);
        return NULL;
    }
    
    void* ptr = calloc(nmemb, size);
    if (!ptr) {
        fprintf(stderr, "[MEMORY] Error: Failed to calloc %zu * %zu bytes at %s:%d\n", nmemb, size, file, line);
        return NULL;
    }
    
#ifdef DEBUG_MEMORY
    track_allocation(ptr, nmemb * size, file, line);
    printf("[MEMORY] Calloc %zu * %zu bytes at %p (%s:%d)\n", nmemb, size, ptr, file, line);
#endif
    
    return ptr;
}

void* safe_realloc(void* ptr, size_t size, const char* file, int line) {
    if (size == 0) {
        // 如果新大小为0，释放原内存
        safe_free(&ptr);
        return NULL;
    }
    
    if (size > 1024 * 1024 * 1024) { // 1GB limit
        fprintf(stderr, "[MEMORY] Error: Realloc size too large (%zu) at %s:%d\n", size, file, line);
        return NULL;
    }
    
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        fprintf(stderr, "[MEMORY] Error: Failed to realloc to %zu bytes at %s:%d\n", size, file, line);
        return NULL;
    }
    
#ifdef DEBUG_MEMORY
    if (ptr != new_ptr) {
        untrack_allocation(ptr);
    }
    track_allocation(new_ptr, size, file, line);
    printf("[MEMORY] Realloc to %zu bytes at %p (%s:%d)\n", size, new_ptr, file, line);
#endif
    
    return new_ptr;
}

void safe_free(void** ptr) {
    if (!ptr || !*ptr) {
        return;
    }
    
#ifdef DEBUG_MEMORY
    untrack_allocation(*ptr);
    printf("[MEMORY] Freed memory at %p\n", *ptr);
#endif
    
    free(*ptr);
    *ptr = NULL;
}

char* safe_strdup(const char* s, const char* file, int line) {
    if (!s) {
        fprintf(stderr, "[MEMORY] Warning: strdup NULL at %s:%d\n", file, line);
        return NULL;
    }
    
    size_t len = strlen(s);
    char* dup = (char*)safe_malloc(len + 1, file, line);
    if (!dup) {
        return NULL;
    }
    
    memcpy(dup, s, len + 1);
    return dup;
}

void safe_memzero(void* ptr, size_t size) {
    if (!ptr || size == 0) {
        return;
    }
    
    // 使用volatile防止编译器优化
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (size--) {
        *p++ = 0;
    }
}

bool safe_ptr_valid(const void* ptr) {
    return ptr != NULL;
}

bool safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return false;
    }
    
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        // 源字符串太长，截断
        memcpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
        return false; // 返回false表示发生了截断
    }
    
    memcpy(dest, src, src_len + 1);
    return true;
}

bool safe_strcat(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return false;
    }
    
    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size) {
        return false;
    }
    
    size_t remaining = dest_size - dest_len;
    size_t src_len = strlen(src);
    
    if (src_len >= remaining) {
        // 源字符串太长，截断
        memcpy(dest + dest_len, src, remaining - 1);
        dest[dest_size - 1] = '\0';
        return false;
    }
    
    memcpy(dest + dest_len, src, src_len + 1);
    return true;
}

void safe_memory_init(void) {
#ifdef DEBUG_MEMORY
    pthread_mutex_lock(&g_mem_mutex);
    memset(g_allocations, 0, sizeof(g_allocations));
    g_alloc_count = 0;
    pthread_mutex_unlock(&g_mem_mutex);
    printf("[MEMORY] Memory tracking initialized\n");
#endif
}

void safe_memory_cleanup(void) {
#ifdef DEBUG_MEMORY
    pthread_mutex_lock(&g_mem_mutex);
    
    if (g_alloc_count > 0) {
        printf("[MEMORY] Warning: %d allocations not freed:\n", g_alloc_count);
        for (int i = 0; i < MAX_TRACKED_ALLOCS; i++) {
            if (g_allocations[i].active) {
                printf("  - %zu bytes at %p (%s:%d)\n", 
                       g_allocations[i].size, 
                       g_allocations[i].ptr,
                       g_allocations[i].file, 
                       g_allocations[i].line);
            }
        }
    } else {
        printf("[MEMORY] All allocations properly freed\n");
    }
    
    pthread_mutex_unlock(&g_mem_mutex);
#endif
}

int safe_memory_get_unfreed_count(void) {
#ifdef DEBUG_MEMORY
    return g_alloc_count;
#else
    return 0;
#endif
}
