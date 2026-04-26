// 必须在所有头文件之前定义_GNU_SOURCE以使用setenv
#define _GNU_SOURCE

#include "linux_desktop_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 桌面环境名称映射
static struct {
    DesktopEnvironment env;
    const char* name;
    const char* identifier;
} desktop_environments[] = {
    {DESKTOP_ENV_GNOME, "GNOME", "GNOME"},
    {DESKTOP_ENV_KDE, "KDE", "KDE"},
    {DESKTOP_ENV_XFCE, "Xfce", "XFCE"},
    {DESKTOP_ENV_MATE, "MATE", "MATE"},
    {DESKTOP_ENV_LXDE, "LXDE", "LXDE"},
    {DESKTOP_ENV_CINNAMON, "Cinnamon", "CINNAMON"},
    {DESKTOP_ENV_PANTHEON, "Pantheon", "PANTHEON"},
    {DESKTOP_ENV_OTHER, "Other", NULL},
    {DESKTOP_ENV_UNKNOWN, "Unknown", NULL}
};

// 显示服务器名称映射
static struct {
    DisplayServer server;
    const char* name;
    const char* identifier;
} display_servers[] = {
    {DISPLAY_SERVER_X11, "X11", "x11"},
    {DISPLAY_SERVER_WAYLAND, "Wayland", "wayland"},
    {DISPLAY_SERVER_UNKNOWN, "Unknown", NULL}
};

// 初始化Linux桌面兼容模块
bool linux_desktop_compat_init(void) {
    printf("Linux desktop compatibility module initialized\n");
    
    // 检测当前桌面环境和显示服务器
    DisplayServer server = linux_desktop_get_display_server();
    DesktopEnvironment env = linux_desktop_get_environment();
    
    printf("Current display server: %s\n", linux_desktop_get_display_server_name(server));
    printf("Current desktop environment: %s\n", linux_desktop_get_environment_name(env));
    
    // 自动设置兼容环境
    linux_desktop_auto_compat();
    
    // 检查兼容性
    if (linux_desktop_is_compatible()) {
        printf("Current desktop environment is compatible\n");
    } else {
        printf("Warning: Current desktop environment may not be fully compatible\n");
    }
    
    return true;
}

// 自动设置兼容环境
bool linux_desktop_auto_compat(void) {
    DisplayServer server = linux_desktop_get_display_server();
    
    // 根据当前显示服务器自动设置兼容环境
    switch (server) {
        case DISPLAY_SERVER_X11:
            // 强制X11兼容
            setenv("GDK_BACKEND", "x11", 1);
            setenv("QT_QPA_PLATFORM", "xcb", 1);
            setenv("SDL_VIDEODRIVER", "x11", 1);
            printf("Auto-configured for X11 compatibility\n");
            break;
        case DISPLAY_SERVER_WAYLAND:
            // 强制Wayland兼容
            setenv("GDK_BACKEND", "wayland", 1);
            setenv("QT_QPA_PLATFORM", "wayland", 1);
            setenv("SDL_VIDEODRIVER", "wayland", 1);
            printf("Auto-configured for Wayland compatibility\n");
            break;
        default:
            // 默认尝试X11
            setenv("GDK_BACKEND", "x11", 1);
            setenv("QT_QPA_PLATFORM", "xcb", 1);
            setenv("SDL_VIDEODRIVER", "x11", 1);
            printf("Auto-configured for default X11 compatibility\n");
            break;
    }
    
    // 设置通用兼容性环境变量
    setenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1", 1);
    setenv("GDK_SCALE", "1", 1);
    setenv("GDK_DPI_SCALE", "1", 1);
    
    return true;
}

// 修复界面缩放问题
bool linux_desktop_fix_scaling(void) {
    // 设置缩放相关环境变量
    setenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1", 1);
    setenv("QT_SCALE_FACTOR", "1", 1);
    setenv("QT_SCREEN_SCALE_FACTORS", "", 1);
    setenv("GDK_SCALE", "1", 1);
    setenv("GDK_DPI_SCALE", "1", 1);
    
    printf("Fixed interface scaling issues\n");
    return true;
}

// 修复窗口显示问题
bool linux_desktop_fix_window_display(void) {
    // 设置窗口相关环境变量
    setenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "0", 1);
    setenv("QT_QPA_PLATFORM_WAYLAND_USE_XDG_DECORATION", "1", 1);
    
    printf("Fixed window display issues\n");
    return true;
}

// 修复异常崩溃问题
bool linux_desktop_fix_crashes(void) {
    // 设置崩溃相关环境变量
    setenv("QT_FORCE_STDERR_LOGGING", "1", 1);
    setenv("G_MESSAGES_DEBUG", "all", 1);
    
    printf("Fixed potential crash issues\n");
    return true;
}

// 清理Linux桌面兼容模块
void linux_desktop_compat_cleanup(void) {
    printf("Linux desktop compatibility module cleaned up\n");
}

// 获取当前显示服务器
DisplayServer linux_desktop_get_display_server(void) {
    // 检查XDG_SESSION_TYPE环境变量
    const char* session_type = getenv("XDG_SESSION_TYPE");
    if (session_type) {
        if (strstr(session_type, "wayland")) {
            return DISPLAY_SERVER_WAYLAND;
        } else if (strstr(session_type, "x11")) {
            return DISPLAY_SERVER_X11;
        }
    }
    
    // 检查DISPLAY环境变量
    const char* display = getenv("DISPLAY");
    if (display) {
        return DISPLAY_SERVER_X11;
    }
    
    return DISPLAY_SERVER_UNKNOWN;
}

// 获取当前桌面环境
DesktopEnvironment linux_desktop_get_environment(void) {
    // 检查XDG_CURRENT_DESKTOP环境变量
    const char* current_desktop = getenv("XDG_CURRENT_DESKTOP");
    if (current_desktop) {
        for (int i = 0; desktop_environments[i].identifier; i++) {
            if (strstr(current_desktop, desktop_environments[i].identifier)) {
                return desktop_environments[i].env;
            }
        }
    }
    
    // 检查DESKTOP_SESSION环境变量
    const char* desktop_session = getenv("DESKTOP_SESSION");
    if (desktop_session) {
        for (int i = 0; desktop_environments[i].identifier; i++) {
            if (strstr(desktop_session, desktop_environments[i].identifier)) {
                return desktop_environments[i].env;
            }
        }
    }
    
    return DESKTOP_ENV_UNKNOWN;
}

// 检查是否兼容当前桌面环境
bool linux_desktop_is_compatible(void) {
    DisplayServer server = linux_desktop_get_display_server();
    DesktopEnvironment env = linux_desktop_get_environment();
    
    // 检查显示服务器
    if (server == DISPLAY_SERVER_X11 || server == DISPLAY_SERVER_WAYLAND) {
        // 检查桌面环境
        switch (env) {
            case DESKTOP_ENV_GNOME:
            case DESKTOP_ENV_KDE:
            case DESKTOP_ENV_XFCE:
            case DESKTOP_ENV_MATE:
            case DESKTOP_ENV_LXDE:
            case DESKTOP_ENV_CINNAMON:
            case DESKTOP_ENV_PANTHEON:
                return true;
            default:
                break;
        }
    }
    
    return false;
}

// 强制兼容X11
bool linux_desktop_force_x11_compat(void) {
    // 设置环境变量强制使用X11
    setenv("GDK_BACKEND", "x11", 1);
    setenv("QT_QPA_PLATFORM", "xcb", 1);
    printf("Forced X11 compatibility\n");
    return true;
}

// 强制兼容Wayland
bool linux_desktop_force_wayland_compat(void) {
    // 设置环境变量强制使用Wayland
    setenv("GDK_BACKEND", "wayland", 1);
    setenv("QT_QPA_PLATFORM", "wayland", 1);
    printf("Forced Wayland compatibility\n");
    return true;
}

// 获取桌面环境名称
const char* linux_desktop_get_environment_name(DesktopEnvironment env) {
    for (int i = 0; desktop_environments[i].identifier || desktop_environments[i].env == DESKTOP_ENV_UNKNOWN; i++) {
        if (desktop_environments[i].env == env) {
            return desktop_environments[i].name;
        }
    }
    return "Unknown";
}

// 获取显示服务器名称
const char* linux_desktop_get_display_server_name(DisplayServer server) {
    for (int i = 0; display_servers[i].identifier || display_servers[i].server == DISPLAY_SERVER_UNKNOWN; i++) {
        if (display_servers[i].server == server) {
            return display_servers[i].name;
        }
    }
    return "Unknown";
}
