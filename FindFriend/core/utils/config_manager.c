// FindFriend 配置管理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CONFIG_FILE "./findfriend.conf"
#define MAX_CONFIG_ITEMS 1024

// 配置项结构
typedef struct {
    char key[64];
    char value[256];
} ConfigItem;

static ConfigItem g_config_items[MAX_CONFIG_ITEMS];
static size_t g_config_count = 0;
static bool g_config_initialized = false;

// 前向声明
FFResult ff_config_manager_load(const char* config_file);
FFResult ff_config_manager_set_defaults(void);
FFResult ff_config_manager_set(const char* key, const char* value);

FFResult ff_config_manager_init(const char* config_file) {
    // 初始化配置项
    g_config_count = 0;
    memset(g_config_items, 0, sizeof(g_config_items));
    
    // 加载配置文件
    const char* file = config_file ? config_file : CONFIG_FILE;
    FFResult res = ff_config_manager_load(file);
    if (res != FF_OK) {
        // 如果加载失败，使用默认配置
        ff_config_manager_set_defaults();
    }
    
    g_config_initialized = true;
    FF_LOG_INFO("Config manager initialized");
    return FF_OK;
}

void ff_config_manager_cleanup(void) {
    g_config_count = 0;
    g_config_initialized = false;
    FF_LOG_INFO("Config manager cleanup");
}

FFResult ff_config_manager_set_defaults(void) {
    // 设置默认配置
    ff_config_manager_set("server.port", "8080");
    ff_config_manager_set("server.host", "0.0.0.0");
    ff_config_manager_set("p2p.port", "5000");
    ff_config_manager_set("p2p.discovery_interval", "5");
    ff_config_manager_set("log.level", "info");
    ff_config_manager_set("log.file", "findfriend.log");
    ff_config_manager_set("security.enabled", "true");
    ff_config_manager_set("cache.size", "1048576");
    ff_config_manager_set("data.dir", "./findfriend_data");
    
    FF_LOG_INFO("Default configuration set");
    return FF_OK;
}

FFResult ff_config_manager_set(const char* key, const char* value) {
    if (!key || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_config_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    // 查找现有配置项
    for (size_t i = 0; i < g_config_count; i++) {
        if (strcmp(g_config_items[i].key, key) == 0) {
            // 更新配置项
            strncpy(g_config_items[i].value, value, sizeof(g_config_items[i].value) - 1);
            FF_LOG_INFO("Config updated: %s = %s", key, value);
            return FF_OK;
        }
    }
    
    // 添加新配置项
    if (g_config_count >= MAX_CONFIG_ITEMS) {
        return FF_ERROR_NO_MEMORY;
    }
    
    ConfigItem* item = &g_config_items[g_config_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    strncpy(item->value, value, sizeof(item->value) - 1);
    
    g_config_count++;
    FF_LOG_INFO("Config set: %s = %s", key, value);
    return FF_OK;
}

FFResult ff_config_manager_get(const char* key, const char** value) {
    if (!key || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_config_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_config_count; i++) {
        if (strcmp(g_config_items[i].key, key) == 0) {
            *value = g_config_items[i].value;
            return FF_OK;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_config_manager_get_int(const char* key, int* value) {
    if (!key || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    const char* str_value = NULL;
    FFResult res = ff_config_manager_get(key, &str_value);
    if (res != FF_OK) {
        return res;
    }
    
    *value = atoi(str_value);
    return FF_OK;
}

FFResult ff_config_manager_get_bool(const char* key, bool* value) {
    if (!key || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    const char* str_value = NULL;
    FFResult res = ff_config_manager_get(key, &str_value);
    if (res != FF_OK) {
        return res;
    }
    
    *value = (strcmp(str_value, "true") == 0 || strcmp(str_value, "1") == 0);
    return FF_OK;
}

FFResult ff_config_manager_get_float(const char* key, float* value) {
    if (!key || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    const char* str_value = NULL;
    FFResult res = ff_config_manager_get(key, &str_value);
    if (res != FF_OK) {
        return res;
    }
    
    *value = atof(str_value);
    return FF_OK;
}

FFResult ff_config_manager_delete(const char* key) {
    if (!key) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_config_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    for (size_t i = 0; i < g_config_count; ) {
        if (strcmp(g_config_items[i].key, key) == 0) {
            memmove(&g_config_items[i], &g_config_items[i + 1], 
                    (g_config_count - i - 1) * sizeof(ConfigItem));
            g_config_count--;
            FF_LOG_INFO("Config deleted: %s", key);
            return FF_OK;
        } else {
            i++;
        }
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_config_manager_load(const char* config_file) {
    if (!config_file) {
        config_file = CONFIG_FILE;
    }
    
    FILE* fp = fopen(config_file, "r");
    if (!fp) {
        FF_LOG_WARN("Config file not found: %s", config_file);
        return FF_ERROR_NOT_FOUND;
    }
    
    char line[512];
    size_t line_num = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        // 解析配置项
        char* equals = strchr(line, '=');
        if (!equals) {
            FF_LOG_WARN("Invalid config line %zu: %s", line_num, line);
            continue;
        }
        
        // 提取键值
        *equals = '\0';
        char* key = line;
        char* value = equals + 1;
        
        // 去除空白字符
        while (*key && isspace((unsigned char)*key)) {
            key++;
        }
        while (*value && isspace((unsigned char)*value)) {
            value++;
        }
        
        // 去除末尾的换行符
        size_t value_len = strlen(value);
        if (value_len > 0 && (value[value_len - 1] == '\n' || value[value_len - 1] == '\r')) {
            value[value_len - 1] = '\0';
        }
        
        // 添加配置项
        if (*key && *value) {
            ff_config_manager_set(key, value);
        }
    }
    
    fclose(fp);
    FF_LOG_INFO("Config loaded from: %s", config_file);
    return FF_OK;
}

FFResult ff_config_manager_save(const char* config_file) {
    if (!g_config_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    if (!config_file) {
        config_file = CONFIG_FILE;
    }
    
    FILE* fp = fopen(config_file, "w");
    if (!fp) {
        FF_LOG_ERROR("Failed to open config file for writing: %s", config_file);
        return FF_ERROR_UNKNOWN;
    }
    
    // 写入配置项
    for (size_t i = 0; i < g_config_count; i++) {
        fprintf(fp, "%s=%s\n", g_config_items[i].key, g_config_items[i].value);
    }
    
    fclose(fp);
    FF_LOG_INFO("Config saved to: %s", config_file);
    return FF_OK;
}

FFResult ff_config_manager_get_all(char*** keys, char*** values, size_t* count) {
    if (!keys || !values || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_config_initialized) {
        *keys = NULL;
        *values = NULL;
        *count = 0;
        return FF_OK;
    }
    
    if (g_config_count == 0) {
        *keys = NULL;
        *values = NULL;
        *count = 0;
        return FF_OK;
    }
    
    *keys = (char**)malloc(g_config_count * sizeof(char*));
    *values = (char**)malloc(g_config_count * sizeof(char*));
    if (!*keys || !*values) {
        if (*keys) {
            free(*keys);
        }
        if (*values) {
            free(*values);
        }
        *keys = NULL;
        *values = NULL;
        *count = 0;
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_config_count; i++) {
        (*keys)[i] = strdup(g_config_items[i].key);
        (*values)[i] = strdup(g_config_items[i].value);
        
        if (!(*keys)[i] || !(*values)[i]) {
            // 清理已分配的内存
            for (size_t j = 0; j < i; j++) {
                free((*keys)[j]);
                free((*values)[j]);
            }
            free(*keys);
            free(*values);
            *keys = NULL;
            *values = NULL;
            *count = 0;
            return FF_ERROR_NO_MEMORY;
        }
    }
    
    *count = g_config_count;
    return FF_OK;
}

FFResult ff_config_manager_free_all(char** keys, char** values, size_t count) {
    if (keys) {
        for (size_t i = 0; i < count; i++) {
            if (keys[i]) {
                free(keys[i]);
            }
        }
        free(keys);
    }
    
    if (values) {
        for (size_t i = 0; i < count; i++) {
            if (values[i]) {
                free(values[i]);
            }
        }
        free(values);
    }
    
    return FF_OK;
}

size_t ff_config_manager_get_count(void) {
    return g_config_count;
}
