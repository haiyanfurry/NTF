// FindFriend 缓存管理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CACHE_ITEMS 1024
#define DEFAULT_CACHE_SIZE 1024 * 1024 // 1MB

// 缓存项结构
typedef struct {
    char key[64];
    void* data;
    size_t size;
    uint64_t timestamp;
    uint32_t access_count;
} CacheItem;

typedef struct {
    size_t item_count;
    size_t current_size;
    size_t max_size;
} FFCacheStats;

static CacheItem g_cache_items[MAX_CACHE_ITEMS];
static size_t g_cache_count = 0;
static size_t g_cache_size = 0;
static size_t g_max_cache_size = DEFAULT_CACHE_SIZE;
static bool g_cache_initialized = false;

FFResult ff_cache_manager_init(size_t max_size) {
    if (max_size > 0) {
        g_max_cache_size = max_size;
    }
    
    g_cache_count = 0;
    g_cache_size = 0;
    memset(g_cache_items, 0, sizeof(g_cache_items));
    g_cache_initialized = true;
    
    FF_LOG_INFO("Cache manager initialized with max size: %zu bytes", g_max_cache_size);
    return FF_OK;
}

void ff_cache_manager_cleanup(void) {
    // 清理所有缓存项
    for (size_t i = 0; i < g_cache_count; i++) {
        if (g_cache_items[i].data) {
            free(g_cache_items[i].data);
        }
    }
    
    g_cache_count = 0;
    g_cache_size = 0;
    g_cache_initialized = false;
    FF_LOG_INFO("Cache manager cleanup");
}

FFResult ff_cache_manager_set(const char* key, const void* data, size_t size) {
    if (!key || !data) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_cache_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    // 检查缓存大小
    if (size > g_max_cache_size) {
        return FF_ERROR_NO_MEMORY;
    }
    
    // 查找现有缓存项
    for (size_t i = 0; i < g_cache_count; i++) {
        if (strcmp(g_cache_items[i].key, key) == 0) {
            // 更新缓存项
            if (g_cache_items[i].data) {
                g_cache_size -= g_cache_items[i].size;
                free(g_cache_items[i].data);
            }
            
            g_cache_items[i].data = malloc(size);
            if (!g_cache_items[i].data) {
                return FF_ERROR_NO_MEMORY;
            }
            memcpy(g_cache_items[i].data, data, size);
            g_cache_items[i].size = size;
            g_cache_items[i].timestamp = time(NULL);
            g_cache_items[i].access_count++;
            
            g_cache_size += size;
            FF_LOG_INFO("Cache updated: %s (size: %zu)", key, size);
            return FF_OK;
        }
    }
    
    // 检查缓存空间
    while (g_cache_size + size > g_max_cache_size && g_cache_count > 0) {
        // 移除最不常用的缓存项
        size_t least_used = 0;
        for (size_t i = 1; i < g_cache_count; i++) {
            if (g_cache_items[i].access_count < g_cache_items[least_used].access_count ||
                (g_cache_items[i].access_count == g_cache_items[least_used].access_count &&
                 g_cache_items[i].timestamp < g_cache_items[least_used].timestamp)) {
                least_used = i;
            }
        }
        
        g_cache_size -= g_cache_items[least_used].size;
        free(g_cache_items[least_used].data);
        memmove(&g_cache_items[least_used], &g_cache_items[least_used + 1], 
                (g_cache_count - least_used - 1) * sizeof(CacheItem));
        g_cache_count--;
    }
    
    // 添加新缓存项
    if (g_cache_count >= MAX_CACHE_ITEMS) {
        return FF_ERROR_NO_MEMORY;
    }
    
    CacheItem* item = &g_cache_items[g_cache_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->data = malloc(size);
    if (!item->data) {
        return FF_ERROR_NO_MEMORY;
    }
    memcpy(item->data, data, size);
    item->size = size;
    item->timestamp = time(NULL);
    item->access_count = 1;
    
    g_cache_count++;
    g_cache_size += size;
    FF_LOG_INFO("Cache stored: %s (size: %zu)", key, size);
    return FF_OK;
}

FFResult ff_cache_manager_get(const char* key, void* data, size_t* size) {
    if (!key || !data || !size) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_cache_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_cache_count; i++) {
        if (strcmp(g_cache_items[i].key, key) == 0) {
            if (*size < g_cache_items[i].size) {
                *size = g_cache_items[i].size;
                return FF_ERROR_NO_MEMORY;
            }
            memcpy(data, g_cache_items[i].data, g_cache_items[i].size);
            *size = g_cache_items[i].size;
            g_cache_items[i].timestamp = time(NULL);
            g_cache_items[i].access_count++;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_cache_manager_delete(const char* key) {
    if (!key) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_cache_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_cache_count; ) {
        if (strcmp(g_cache_items[i].key, key) == 0) {
            g_cache_size -= g_cache_items[i].size;
            free(g_cache_items[i].data);
            memmove(&g_cache_items[i], &g_cache_items[i + 1], 
                    (g_cache_count - i - 1) * sizeof(CacheItem));
            g_cache_count--;
            FF_LOG_INFO("Cache deleted: %s", key);
            return FF_OK;
        } else {
            i++;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_cache_manager_exists(const char* key, bool* exists) {
    if (!key || !exists) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_cache_initialized) {
        *exists = false;
        return FF_OK;
    }
    
    for (size_t i = 0; i < g_cache_count; i++) {
        if (strcmp(g_cache_items[i].key, key) == 0) {
            *exists = true;
            return FF_OK;
        }
    }
    
    *exists = false;
    return FF_OK;
}

FFResult ff_cache_manager_clear(void) {
    if (!g_cache_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_cache_count; i++) {
        if (g_cache_items[i].data) {
            free(g_cache_items[i].data);
        }
    }
    
    g_cache_count = 0;
    g_cache_size = 0;
    FF_LOG_INFO("Cache cleared");
    return FF_OK;
}

FFResult ff_cache_manager_get_stats(FFCacheStats* stats) {
    if (!stats) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_cache_initialized) {
        memset(stats, 0, sizeof(FFCacheStats));
        return FF_OK;
    }
    
    stats->item_count = g_cache_count;
    stats->current_size = g_cache_size;
    stats->max_size = g_max_cache_size;
    
    return FF_OK;
}

FFResult ff_cache_manager_set_max_size(size_t max_size) {
    if (!g_cache_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    g_max_cache_size = max_size;
    
    // 清理超出大小的缓存项
    while (g_cache_size > g_max_cache_size && g_cache_count > 0) {
        size_t least_used = 0;
        for (size_t i = 1; i < g_cache_count; i++) {
            if (g_cache_items[i].access_count < g_cache_items[least_used].access_count ||
                (g_cache_items[i].access_count == g_cache_items[least_used].access_count &&
                 g_cache_items[i].timestamp < g_cache_items[least_used].timestamp)) {
                least_used = i;
            }
        }
        
        g_cache_size -= g_cache_items[least_used].size;
        free(g_cache_items[least_used].data);
        memmove(&g_cache_items[least_used], &g_cache_items[least_used + 1], 
                (g_cache_count - least_used - 1) * sizeof(CacheItem));
        g_cache_count--;
    }
    
    FF_LOG_INFO("Cache max size set to: %zu bytes", g_max_cache_size);
    return FF_OK;
}

size_t ff_cache_manager_get_count(void) {
    return g_cache_count;
}

size_t ff_cache_manager_get_size(void) {
    return g_cache_size;
}
