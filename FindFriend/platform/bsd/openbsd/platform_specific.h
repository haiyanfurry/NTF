// FindFriend OpenBSD 平台特定接口

#ifndef FF_PLATFORM_OPENBSD_H
#define FF_PLATFORM_OPENBSD_H

#include "ff_core_interface.h"

// 平台初始化
FFResult platform_init(void);

// 平台清理
void platform_cleanup(void);

// 事件轮询
FFResult platform_poll_events(void);

// 系统信息
FFResult platform_get_system_info(char* buffer, size_t buffer_size);

// 网络接口
FFResult platform_get_network_interfaces(char* buffer, size_t buffer_size);

// 进程管理
FFResult platform_get_process_info(char* buffer, size_t buffer_size);

// 文件系统
FFResult platform_get_filesystem_info(char* buffer, size_t buffer_size);

// 内存信息
FFResult platform_get_memory_info(char* buffer, size_t buffer_size);

// 系统负载
FFResult platform_get_system_load(double* load1, double* load5, double* load15);

// 电池状态（如果适用）
FFResult platform_get_battery_info(int* percentage, bool* is_charging);

#endif // FF_PLATFORM_OPENBSD_H
