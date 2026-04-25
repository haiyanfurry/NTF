// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "location_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// 位置响应回调函数
static void location_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    LocationCallback callback = (LocationCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    LocationManager* manager = (LocationManager*)((void**)user_data)[2];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 模拟位置数据
        double latitude = 39.9042;
        double longitude = 116.4074;
        
        // 更新本地位置
        manager->latitude = latitude;
        manager->longitude = longitude;
        
        // 调用回调函数
        if (callback) {
            callback(callback_user_data, status, latitude, longitude);
        }
    } else {
        // 获取位置失败
        if (callback) {
            callback(callback_user_data, status, 0, 0);
        }
    }
    
    // 清理资源
    free(user_data);
}

// 位置更新线程函数
static void* location_update_thread(void* arg) {
    LocationManager* manager = (LocationManager*)arg;
    
    while (manager->updating) {
        // 模拟获取位置
        double latitude = 39.9042 + (rand() % 100) / 10000.0;
        double longitude = 116.4074 + (rand() % 100) / 10000.0;
        
        // 更新本地位置
        manager->latitude = latitude;
        manager->longitude = longitude;
        
        // 发送位置更新
        location_manager_send_location_update(manager, latitude, longitude);
        
        // 调用更新回调
        if (manager->update_callback) {
            manager->update_callback(manager->update_user_data, latitude, longitude);
        }
        
        // 等待一段时间
        sleep(5);
    }
    
    return NULL;
}

// 初始化定位管理器
LocationManager* location_manager_init() {
    LocationManager* manager = (LocationManager*)malloc(sizeof(LocationManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->latitude = 0;
    manager->longitude = 0;
    manager->location_enabled = false;
    manager->updating = false;
    manager->update_callback = NULL;
    manager->update_user_data = NULL;
    
    return manager;
}

// 销毁定位管理器
void location_manager_destroy(LocationManager* manager) {
    if (manager) {
        location_manager_stop_location(manager);
        free(manager);
    }
}

// 开启定位
bool location_manager_start_location(LocationManager* manager) {
    if (!manager) {
        return false;
    }
    
    if (manager->updating) {
        return true;
    }
    
    manager->updating = true;
    manager->location_enabled = true;
    
    // 创建位置更新线程
    pthread_t thread;
    if (pthread_create(&thread, NULL, location_update_thread, manager) != 0) {
        fprintf(stderr, "Error creating location update thread\n");
        manager->updating = false;
        manager->location_enabled = false;
        return false;
    }
    
    // 分离线程，让它自行清理
    pthread_detach(thread);
    
    return true;
}

// 关闭定位
void location_manager_stop_location(LocationManager* manager) {
    if (manager) {
        manager->updating = false;
        manager->location_enabled = false;
    }
}

// 获取当前位置
bool location_manager_get_current_location(LocationManager* manager, LocationCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 3);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    
    // 发送位置请求
    int result = network_report_location(&manager->latitude);
    if (result != NETWORK_SUCCESS) {
        free(callback_data);
        return false;
    }
    
    // 模拟回调
    if (callback) {
        callback(user_data, 200, manager->latitude, manager->longitude);
    }
    
    free(callback_data);
    return true;
}

// 设置位置更新回调
void location_manager_set_update_callback(LocationManager* manager, LocationUpdateCallback callback, void* user_data) {
    if (manager) {
        manager->update_callback = callback;
        manager->update_user_data = user_data;
    }
}

// 发送位置更新
bool location_manager_send_location_update(LocationManager* manager, double latitude, double longitude) {
    if (!manager || !manager->location_enabled) {
        return false;
    }
    
    // 发送位置更新请求
    int result = network_report_location(&latitude);
    return result == NETWORK_SUCCESS;
}

// 检查定位是否开启
bool location_manager_is_location_enabled(LocationManager* manager) {
    if (!manager) {
        return false;
    }
    return manager->location_enabled;
}