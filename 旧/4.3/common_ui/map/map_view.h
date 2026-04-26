#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 地图标记结构体
typedef struct {
    char* marker_id;
    double latitude;
    double longitude;
    char* title;
    char* subtitle;
    bool is_user;
    bool is_exhibition;
} MapMarker;

// 地图视图回调函数类型
typedef void (*MarkerSelectCallback)(void* user_data, const char* marker_id);

// 地图视图结构体
typedef struct {
    void* native_view;
    MapMarker** markers;
    size_t marker_count;
    double current_latitude;
    double current_longitude;
    MarkerSelectCallback callback;
    void* user_data;
} MapView;

// 创建地图视图
MapView* map_view_create(void* parent_window);

// 销毁地图视图
void map_view_destroy(MapView* view);

// 显示地图视图
void map_view_show(MapView* view);

// 隐藏地图视图
void map_view_hide(MapView* view);

// 设置地图标记
void map_view_set_markers(MapView* view, MapMarker** markers, size_t count);

// 添加地图标记
void map_view_add_marker(MapView* view, MapMarker* marker);

// 移除地图标记
void map_view_remove_marker(MapView* view, const char* marker_id);

// 更新地图标记位置
void map_view_update_marker_position(MapView* view, const char* marker_id, double latitude, double longitude);

// 定位到指定位置
void map_view_center_on_location(MapView* view, double latitude, double longitude, double zoom);

// 定位到用户位置
void map_view_center_on_user(MapView* view);

// 设置标记选择回调
void map_view_set_select_callback(MapView* view, MarkerSelectCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // MAP_VIEW_H