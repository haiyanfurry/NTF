// FindFriend 数据存储模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_DATA_ITEMS 1024
#define DATA_DIR "./findfriend_data"

// 数据项结构
typedef struct {
    char key[64];
    char value[4096];
    uint64_t timestamp;
    uint32_t size;
} DataItem;

static DataItem g_data_items[MAX_DATA_ITEMS];
static size_t g_data_count = 0;
static bool g_data_initialized = false;

// 函数声明
FFResult ff_data_store_load(void);
FFResult ff_data_store_save(void);

FFResult ff_data_store_init(void) {
    // 创建数据目录
    if (access(DATA_DIR, F_OK) != 0) {
        if (mkdir(DATA_DIR, 0755) != 0) {
            FF_LOG_ERROR("Failed to create data directory");
            return FF_ERROR_UNKNOWN;
        }
    }
    
    // 初始化数据存储
    g_data_count = 0;
    memset(g_data_items, 0, sizeof(g_data_items));
    g_data_initialized = true;
    
    // 加载数据
    ff_data_store_load();
    
    FF_LOG_INFO("Data store initialized");
    return FF_OK;
}

void ff_data_store_cleanup(void) {
    // 保存数据
    ff_data_store_save();
    
    g_data_count = 0;
    g_data_initialized = false;
    FF_LOG_INFO("Data store cleanup");
}

FFResult ff_data_store_set(const char* key, const void* value, size_t size) {
    if (!key || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_data_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    // 查找现有数据项
    for (size_t i = 0; i < g_data_count; i++) {
        if (strcmp(g_data_items[i].key, key) == 0) {
            // 更新数据项
            if (size > sizeof(g_data_items[i].value)) {
                return FF_ERROR_NO_MEMORY;
            }
            memcpy(g_data_items[i].value, value, size);
            g_data_items[i].size = size;
            g_data_items[i].timestamp = time(NULL);
            FF_LOG_INFO("Data updated: %s", key);
            return FF_OK;
        }
    }
    
    // 添加新数据项
    if (g_data_count >= MAX_DATA_ITEMS) {
        return FF_ERROR_NO_MEMORY;
    }
    
    DataItem* item = &g_data_items[g_data_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    if (size > sizeof(item->value)) {
        return FF_ERROR_NO_MEMORY;
    }
    memcpy(item->value, value, size);
    item->size = size;
    item->timestamp = time(NULL);
    
    g_data_count++;
    FF_LOG_INFO("Data stored: %s", key);
    return FF_OK;
}

FFResult ff_data_store_get(const char* key, void* value, size_t* size) {
    if (!key || !value || !size) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_data_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_data_count; i++) {
        if (strcmp(g_data_items[i].key, key) == 0) {
            if (*size < g_data_items[i].size) {
                *size = g_data_items[i].size;
                return FF_ERROR_NO_MEMORY;
            }
            memcpy(value, g_data_items[i].value, g_data_items[i].size);
            *size = g_data_items[i].size;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_data_store_delete(const char* key) {
    if (!key) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_data_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_data_count; ) {
        if (strcmp(g_data_items[i].key, key) == 0) {
            memmove(&g_data_items[i], &g_data_items[i + 1], 
                    (g_data_count - i - 1) * sizeof(DataItem));
            g_data_count--;
            FF_LOG_INFO("Data deleted: %s", key);
            return FF_OK;
        } else {
            i++;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_data_store_exists(const char* key, bool* exists) {
    if (!key || !exists) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_data_initialized) {
        *exists = false;
        return FF_OK;
    }
    
    for (size_t i = 0; i < g_data_count; i++) {
        if (strcmp(g_data_items[i].key, key) == 0) {
            *exists = true;
            return FF_OK;
        }
    }
    
    *exists = false;
    return FF_OK;
}

FFResult ff_data_store_get_all_keys(char*** keys, size_t* count) {
    if (!keys || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_data_initialized) {
        *keys = NULL;
        *count = 0;
        return FF_OK;
    }
    
    if (g_data_count == 0) {
        *keys = NULL;
        *count = 0;
        return FF_OK;
    }
    
    *keys = (char**)malloc(g_data_count * sizeof(char*));
    if (!*keys) {
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_data_count; i++) {
        (*keys)[i] = strdup(g_data_items[i].key);
        if (!(*keys)[i]) {
            // 清理已分配的内存
            for (size_t j = 0; j < i; j++) {
                free((*keys)[j]);
            }
            free(*keys);
            *keys = NULL;
            *count = 0;
            return FF_ERROR_NO_MEMORY;
        }
    }
    
    *count = g_data_count;
    return FF_OK;
}

FFResult ff_data_store_free_keys(char** keys, size_t count) {
    if (keys) {
        for (size_t i = 0; i < count; i++) {
            if (keys[i]) {
                free(keys[i]);
            }
        }
        free(keys);
    }
    return FF_OK;
}

FFResult ff_data_store_save(void) {
    if (!g_data_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/data.dat", DATA_DIR);
    
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        FF_LOG_ERROR("Failed to open data file for writing");
        return FF_ERROR_UNKNOWN;
    }
    
    // 写入数据项数量
    fwrite(&g_data_count, sizeof(size_t), 1, fp);
    
    // 写入数据项
    for (size_t i = 0; i < g_data_count; i++) {
        fwrite(&g_data_items[i], sizeof(DataItem), 1, fp);
    }
    
    fclose(fp);
    FF_LOG_INFO("Data saved to disk");
    return FF_OK;
}

FFResult ff_data_store_load(void) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/data.dat", DATA_DIR);
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        // 文件不存在，正常返回
        return FF_OK;
    }
    
    // 读取数据项数量
    size_t count = 0;
    fread(&count, sizeof(size_t), 1, fp);
    
    if (count > MAX_DATA_ITEMS) {
        count = MAX_DATA_ITEMS;
    }
    
    // 读取数据项
    for (size_t i = 0; i < count; i++) {
        fread(&g_data_items[i], sizeof(DataItem), 1, fp);
    }
    
    g_data_count = count;
    fclose(fp);
    FF_LOG_INFO("Data loaded from disk: %zu items", count);
    return FF_OK;
}

FFResult ff_data_store_cleanup_old_data(uint64_t max_age) {
    if (!g_data_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    time_t now = time(NULL);
    size_t removed = 0;
    
    for (size_t i = 0; i < g_data_count; ) {
        if (now - g_data_items[i].timestamp > max_age) {
            memmove(&g_data_items[i], &g_data_items[i + 1], 
                    (g_data_count - i - 1) * sizeof(DataItem));
            g_data_count--;
            removed++;
        } else {
            i++;
        }
    }
    
    if (removed > 0) {
        FF_LOG_INFO("Cleaned up %zu old data items", removed);
    }
    
    return FF_OK;
}

size_t ff_data_store_get_count(void) {
    return g_data_count;
}
