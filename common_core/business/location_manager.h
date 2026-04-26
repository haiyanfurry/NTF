#ifndef LOCATION_MANAGER_H
#define LOCATION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 定位回调函数类型
typedef void (*LocationCallback)(void* user_data, int status, double latitude, double longitude);

// 位置更新回调函数类型
typedef void (*LocationUpdateCallback)(void* user_data, double latitude, double longitude);

// 定位管理器结构体
typedef struct {
    void* network_manager;
    double latitude;
    double longitude;
    bool location_enabled;
    bool updating;
    LocationUpdateCallback update_callback;
    void* update_user_data;
} LocationManager;

// 初始化定位管理器
LocationManager* location_manager_init();

// 销毁定位管理器
void location_manager_destroy(LocationManager* manager);

// 开启定位
bool location_manager_start_location(LocationManager* manager);

// 关闭定位
void location_manager_stop_location(LocationManager* manager);

// 获取当前位置
bool location_manager_get_current_location(LocationManager* manager, LocationCallback callback, void* user_data);

// 设置位置更新回调
void location_manager_set_update_callback(LocationManager* manager, LocationUpdateCallback callback, void* user_data);

// 发送位置更新
bool location_manager_send_location_update(LocationManager* manager, double latitude, double longitude);

// 检查定位是否开启
bool location_manager_is_location_enabled(LocationManager* manager);

#ifdef __cplusplus
}
#endif

#endif // LOCATION_MANAGER_H