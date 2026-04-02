// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "exhibition_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
// 展商响应回调函数
static void exhibition_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    ExhibitionCallback callback = (ExhibitionCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    ExhibitionManager* manager = (ExhibitionManager*)((void**)user_data)[2];
    const char* action = (const char*)((void**)user_data)[3];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 根据操作类型处理响应
        if (strcmp(action, "get_exhibitions") == 0) {
            // 模拟展商列表
            // 实际应用中应该解析服务器返回的展商列表
            if (manager->exhibitions) {
                free(manager->exhibitions);
            }
            manager->exhibition_count = 2;
            manager->exhibitions = (Exhibition*)malloc(sizeof(Exhibition) * manager->exhibition_count);
            if (manager->exhibitions) {
                // 展商 1
                manager->exhibitions[0].exhibition_id = strdup("exhibition_123");
                manager->exhibitions[0].name = strdup("测试展会 1");
                manager->exhibitions[0].description = strdup("这是一个测试展会");
                manager->exhibitions[0].url = strdup("http://example.com/exhibition1");
                manager->exhibitions[0].start_time = strdup("2026-04-01 10:00:00");
                manager->exhibitions[0].end_time = strdup("2026-04-02 18:00:00");
                manager->exhibitions[0].location = strdup("北京市朝阳区");
                manager->exhibitions[0].booth_count = 50;
                manager->exhibitions[0].is_active = true;
                
                // 展商 2
                manager->exhibitions[1].exhibition_id = strdup("exhibition_456");
                manager->exhibitions[1].name = strdup("测试展会 2");
                manager->exhibitions[1].description = strdup("这是另一个测试展会");
                manager->exhibitions[1].url = strdup("http://example.com/exhibition2");
                manager->exhibitions[1].start_time = strdup("2026-05-01 10:00:00");
                manager->exhibitions[1].end_time = strdup("2026-05-02 18:00:00");
                manager->exhibitions[1].location = strdup("上海市浦东新区");
                manager->exhibitions[1].booth_count = 100;
                manager->exhibitions[1].is_active = true;
            }
        } else if (strcmp(action, "get_exhibition_detail") == 0) {
            printf("Exhibition detail retrieved successfully\n");
        } else if (strcmp(action, "create_exhibition") == 0) {
            printf("Exhibition created successfully\n");
        } else if (strcmp(action, "update_exhibition") == 0) {
            printf("Exhibition updated successfully\n");
        } else if (strcmp(action, "delete_exhibition") == 0) {
            printf("Exhibition deleted successfully\n");
        } else if (strcmp(action, "search_exhibition") == 0) {
            printf("Exhibition searched successfully\n");
        }
        
        // 调用回调函数
        if (callback) {
            callback(callback_user_data, status, manager->exhibitions);
        }
    } else {
        // 操作失败
        if (callback) {
            callback(callback_user_data, status, NULL);
        }
    }
    
    // 清理资源
    free((void*)action);
    free(user_data);
}

// 初始化展商管理器
ExhibitionManager* exhibition_manager_init() {
    ExhibitionManager* manager = (ExhibitionManager*)malloc(sizeof(ExhibitionManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->exhibitions = NULL;
    manager->exhibition_count = 0;
    
    return manager;
}

// 销毁展商管理器
void exhibition_manager_destroy(ExhibitionManager* manager) {
    if (manager) {
        // 释放展商列表
        if (manager->exhibitions) {
            for (size_t i = 0; i < manager->exhibition_count; i++) {
                if (manager->exhibitions[i].exhibition_id) free(manager->exhibitions[i].exhibition_id);
                if (manager->exhibitions[i].name) free(manager->exhibitions[i].name);
                if (manager->exhibitions[i].description) free(manager->exhibitions[i].description);
                if (manager->exhibitions[i].url) free(manager->exhibitions[i].url);
                if (manager->exhibitions[i].start_time) free(manager->exhibitions[i].start_time);
                if (manager->exhibitions[i].end_time) free(manager->exhibitions[i].end_time);
                if (manager->exhibitions[i].location) free(manager->exhibitions[i].location);
            }
            free(manager->exhibitions);
        }
        free(manager);
    }
}

// 获取展商列表
bool exhibition_manager_get_exhibitions(ExhibitionManager* manager, ExhibitionCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("get_exhibitions");
    
    // 模拟获取展商列表请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting exhibitions\n");
    
    // 模拟请求成功
    exhibition_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 获取展商详情
bool exhibition_manager_get_exhibition_detail(ExhibitionManager* manager, const char* exhibition_id, ExhibitionCallback callback, void* user_data) {
    if (!manager || !exhibition_id) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("get_exhibition_detail");
    
    // 模拟获取展商详情请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting exhibition detail for ID: %s\n", exhibition_id);
    
    // 模拟请求成功
    exhibition_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 创建展商计划
bool exhibition_manager_create_exhibition(ExhibitionManager* manager, Exhibition* exhibition, ExhibitionCallback callback, void* user_data) {
    if (!manager || !exhibition) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("create_exhibition");
    
    // 构建展商数据
    char* data = (char*)malloc(1024);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 1024, "{\"name\": \"%s\", \"description\": \"%s\", \"url\": \"%s\", \"start_time\": \"%s\", \"end_time\": \"%s\", \"location\": \"%s\", \"booth_count\": %d}", 
             exhibition->name, exhibition->description, exhibition->url, exhibition->start_time, exhibition->end_time, exhibition->location, exhibition->booth_count);
    
    // 模拟创建展商计划请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Creating exhibition: %s\n", exhibition->name);
    
    // 模拟请求成功
    exhibition_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 更新展商计划
bool exhibition_manager_update_exhibition(ExhibitionManager* manager, Exhibition* exhibition, ExhibitionCallback callback, void* user_data) {
    if (!manager || !exhibition || !exhibition->exhibition_id) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("update_exhibition");
    
    // 构建展商数据
    char* data = (char*)malloc(1024);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 1024, "{\"exhibition_id\": \"%s\", \"name\": \"%s\", \"description\": \"%s\", \"url\": \"%s\", \"start_time\": \"%s\", \"end_time\": \"%s\", \"location\": \"%s\", \"booth_count\": %d, \"is_active\": %s}", 
             exhibition->exhibition_id, exhibition->name, exhibition->description, exhibition->url, exhibition->start_time, exhibition->end_time, exhibition->location, exhibition->booth_count, exhibition->is_active ? "true" : "false");
    
    // 模拟更新展商计划请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Updating exhibition: %s\n", exhibition->name);
    
    // 模拟请求成功
    exhibition_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 删除展商计划
bool exhibition_manager_delete_exhibition(ExhibitionManager* manager, const char* exhibition_id, ExhibitionCallback callback, void* user_data) {
    if (!manager || !exhibition_id) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("delete_exhibition");
    
    // 构建删除请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"exhibition_id\": \"%s\"}", exhibition_id);
    
    // 模拟删除展商计划请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Deleting exhibition with ID: %s\n", exhibition_id);
    
    // 模拟请求成功
    exhibition_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 搜索展商
bool exhibition_manager_search_exhibition(ExhibitionManager* manager, const char* keyword, ExhibitionCallback callback, void* user_data) {
    if (!manager || !keyword) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("search_exhibition");
    
    // 构建搜索请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"keyword\": \"%s\"}", keyword);
    
    // 模拟搜索展商请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Searching exhibitions with keyword: %s\n", keyword);
    
    // 模拟请求成功
    exhibition_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}