#ifndef LINUX_DESKTOP_COMPAT_H
#define LINUX_DESKTOP_COMPAT_H

#include <stdbool.h>
#include <stdint.h>

// 桌面环境类型
typedef enum {
    DESKTOP_ENV_UNKNOWN,
    DESKTOP_ENV_GNOME,
    DESKTOP_ENV_KDE,
    DESKTOP_ENV_XFCE,
    DESKTOP_ENV_MATE,
    DESKTOP_ENV_LXDE,
    DESKTOP_ENV_CINNAMON,
    DESKTOP_ENV_PANTHEON,
    DESKTOP_ENV_OTHER
} DesktopEnvironment;

// 显示服务器类型
typedef enum {
    DISPLAY_SERVER_UNKNOWN,
    DISPLAY_SERVER_X11,
    DISPLAY_SERVER_WAYLAND
} DisplayServer;

// 初始化Linux桌面兼容模块
bool linux_desktop_compat_init(void);

// 清理Linux桌面兼容模块
void linux_desktop_compat_cleanup(void);

// 获取当前显示服务器
DisplayServer linux_desktop_get_display_server(void);

// 获取当前桌面环境
DesktopEnvironment linux_desktop_get_environment(void);

// 检查是否兼容当前桌面环境
bool linux_desktop_is_compatible(void);

// 强制兼容X11
bool linux_desktop_force_x11_compat(void);

// 强制兼容Wayland
bool linux_desktop_force_wayland_compat(void);

// 自动设置兼容环境
bool linux_desktop_auto_compat(void);

// 修复界面缩放问题
bool linux_desktop_fix_scaling(void);

// 修复窗口显示问题
bool linux_desktop_fix_window_display(void);

// 修复异常崩溃问题
bool linux_desktop_fix_crashes(void);

// 获取桌面环境名称
const char* linux_desktop_get_environment_name(DesktopEnvironment env);

// 获取显示服务器名称
const char* linux_desktop_get_display_server_name(DisplayServer server);

#endif // LINUX_DESKTOP_COMPAT_H
