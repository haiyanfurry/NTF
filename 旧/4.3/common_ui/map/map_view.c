#include "map_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建地图视图
MapView* map_view_create(void* parent_window) {
    MapView* view = (MapView*)malloc(sizeof(MapView));
    if (!view) {
        fprintf(stderr, "Error allocating memory for map view\n");
        return NULL;
    }
    
    // 初始化结构体
    view->native_view = NULL; // 由平台特定实现设置
    view->markers = NULL;
    view->marker_count = 0;
    view->current_latitude = 0.0;
    view->current_longitude = 0.0;
    view->callback = NULL;
    view->user_data = NULL;
    
    return view;
}

// 销毁地图视图
void map_view_destroy(MapView* view) {
    if (view) {
        // 释放标记列表
        if (view->markers) {
            for (size_t i = 0; i < view->marker_count; i++) {
                if (view->markers[i]) {
                    free(view->markers[i]->marker_id);
                    free(view->markers[i]->title);
                    free(view->markers[i]->subtitle);
                    free(view->markers[i]);
                }
            }
            free(view->markers);
        }
        
        // 平台特定实现应该释放 native_view
        free(view);
    }
}

// 显示地图视图
void map_view_show(MapView* view) {
    if (!view) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Showing map view\n");
}

// 隐藏地图视图
void map_view_hide(MapView* view) {
    if (!view) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Hiding map view\n");
}

// 设置地图标记
void map_view_set_markers(MapView* view, MapMarker** markers, size_t count) {
    if (!view) {
        return;
    }
    
    // 释放旧的标记列表
    if (view->markers) {
        for (size_t i = 0; i < view->marker_count; i++) {
            if (view->markers[i]) {
                free(view->markers[i]->marker_id);
                free(view->markers[i]->title);
                free(view->markers[i]->subtitle);
                free(view->markers[i]);
            }
        }
        free(view->markers);
    }
    
    // 设置新的标记列表
    view->markers = markers;
    view->marker_count = count;
    
    // 由平台特定实现更新UI
    printf("Set map markers with %zu markers\n", count);
}

// 添加地图标记
void map_view_add_marker(MapView* view, MapMarker* marker) {
    if (!view || !marker) {
        return;
    }
    
    // 重新分配内存
    MapMarker** new_markers = (MapMarker**)realloc(view->markers, (view->marker_count + 1) * sizeof(MapMarker*));
    if (!new_markers) {
        fprintf(stderr, "Error reallocating memory for marker list\n");
        return;
    }
    
    // 添加新标记
    new_markers[view->marker_count] = marker;
    view->markers = new_markers;
    view->marker_count++;
    
    // 由平台特定实现更新UI
    printf("Added marker: %s at (%.6f, %.6f)\n", marker->title, marker->latitude, marker->longitude);
}

// 移除地图标记
void map_view_remove_marker(MapView* view, const char* marker_id) {
    if (!view || !marker_id) {
        return;
    }
    
    // 查找标记
    size_t index = -1;
    for (size_t i = 0; i < view->marker_count; i++) {
        if (strcmp(view->markers[i]->marker_id, marker_id) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return;
    }
    
    // 释放标记内存
    free(view->markers[index]->marker_id);
    free(view->markers[index]->title);
    free(view->markers[index]->subtitle);
    free(view->markers[index]);
    
    // 移动其他标记
    for (size_t i = index; i < view->marker_count - 1; i++) {
        view->markers[i] = view->markers[i + 1];
    }
    
    // 重新分配内存
    MapMarker** new_markers = (MapMarker**)realloc(view->markers, (view->marker_count - 1) * sizeof(MapMarker*));
    if (new_markers || view->marker_count == 1) {
        view->markers = new_markers;
        view->marker_count--;
    }
    
    // 由平台特定实现更新UI
    printf("Removed marker: %s\n", marker_id);
}

// 更新地图标记位置
void map_view_update_marker_position(MapView* view, const char* marker_id, double latitude, double longitude) {
    if (!view || !marker_id) {
        return;
    }
    
    // 查找标记
    for (size_t i = 0; i < view->marker_count; i++) {
        if (strcmp(view->markers[i]->marker_id, marker_id) == 0) {
            view->markers[i]->latitude = latitude;
            view->markers[i]->longitude = longitude;
            // 由平台特定实现更新UI
            printf("Updated marker %s position to (%.6f, %.6f)\n", marker_id, latitude, longitude);
            break;
        }
    }
}

// 定位到指定位置
void map_view_center_on_location(MapView* view, double latitude, double longitude, double zoom) {
    if (!view) {
        return;
    }
    
    // 更新当前位置
    view->current_latitude = latitude;
    view->current_longitude = longitude;
    
    // 由平台特定实现实现
    printf("Centering map on location (%.6f, %.6f) with zoom %.2f\n", latitude, longitude, zoom);
}

// 定位到用户位置
void map_view_center_on_user(MapView* view) {
    if (!view) {
        return;
    }
    
    // 由平台特定实现实现
    printf("Centering map on user location\n");
}

// 设置标记选择回调
void map_view_set_select_callback(MapView* view, MarkerSelectCallback callback, void* user_data) {
    if (!view) {
        return;
    }
    
    view->callback = callback;
    view->user_data = user_data;
}