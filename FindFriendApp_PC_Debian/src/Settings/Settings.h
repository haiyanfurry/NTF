#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

typedef struct {
    int good_samaritan_mode;      // 老好人模式开关
    int server_port;               // 服务器端口
    double map_zoom;               // 地图缩放级别
    double map_lat;                // 地图中心纬度
    double map_lng;                // 地图中心经度
    char nickname[64];             // 用户昵称
    char signature[256];           // 个性签名
    uint16_t tags;                 // 用户标签
    int animation_enabled;         // 动画开关
} AppSettings;

// 加载/保存设置
void settings_load(AppSettings *s);
void settings_save(AppSettings *s);
void settings_init(AppSettings *s);

#endif
