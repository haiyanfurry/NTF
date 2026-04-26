// FindFriend FreeBSD 平台特定实现

#include "platform_specific.h"
#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/statvfs.h>
#include <sys/resource.h>
#include <sys/vmmeter.h>
#include <sys/loadavg.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>

static bool g_initialized = false;

// 平台初始化
FFResult platform_init(void) {
    g_initialized = true;
    FF_INFO("FreeBSD platform initialized");
    return FF_OK;
}

// 平台清理
void platform_cleanup(void) {
    if (g_initialized) {
        FF_INFO("FreeBSD platform cleanup");
        g_initialized = false;
    }
}

// 事件轮询
FFResult platform_poll_events(void) {
    // FreeBSD 事件轮询实现
    return FF_OK;
}

// 系统信息
FFResult platform_get_system_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        snprintf(buffer, buffer_size, "Hostname: %s\n", hostname);
    }
    
    int version[2];
    size_t len = sizeof(version);
    if (sysctlbyname("kern.osreldate", &version, &len, NULL, 0) == 0) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "OS Version: FreeBSD %d\n", version[0]);
    }
    
    return FF_OK;
}

// 网络接口
FFResult platform_get_network_interfaces(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        return FF_ERROR_NETWORK;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            char host[NI_MAXHOST];
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                           host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
                snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer),
                        "Interface: %s, IP: %s\n", ifa->ifa_name, host);
            }
        }
    }
    
    freeifaddrs(ifaddr);
    return FF_OK;
}

// 进程管理
FFResult platform_get_process_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    int pid = getpid();
    snprintf(buffer, buffer_size, "PID: %d\n", pid);
    
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer),
                "CPU Time: %ld.%06ld sec\n", 
                usage.ru_utime.tv_sec + usage.ru_stime.tv_sec,
                usage.ru_utime.tv_usec + usage.ru_stime.tv_usec);
    }
    
    return FF_OK;
}

// 文件系统
FFResult platform_get_filesystem_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct statvfs statfs;
    if (statvfs("/", &statfs) == 0) {
        unsigned long long total = (unsigned long long)statfs.f_blocks * statfs.f_bsize / (1024 * 1024);
        unsigned long long free = (unsigned long long)statfs.f_bfree * statfs.f_bsize / (1024 * 1024);
        
        snprintf(buffer, buffer_size, "Filesystem: %llu MB total, %llu MB free\n", total, free);
    }
    
    return FF_OK;
}

// 内存信息
FFResult platform_get_memory_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    int pagesize;
    size_t len = sizeof(pagesize);
    if (sysctlbyname("vm.pagesize", &pagesize, &len, NULL, 0) != 0) {
        return FF_ERROR_MEMORY;
    }
    
    int freepages;
    len = sizeof(freepages);
    if (sysctlbyname("vm.stats.vm.v_free_count", &freepages, &len, NULL, 0) != 0) {
        return FF_ERROR_MEMORY;
    }
    
    int activepages;
    len = sizeof(activepages);
    if (sysctlbyname("vm.stats.vm.v_active_count", &activepages, &len, NULL, 0) != 0) {
        return FF_ERROR_MEMORY;
    }
    
    unsigned long long free = (unsigned long long)freepages * pagesize / (1024 * 1024);
    unsigned long long active = (unsigned long long)activepages * pagesize / (1024 * 1024);
    
    snprintf(buffer, buffer_size, "Memory: %llu MB active, %llu MB free\n", active, free);
    
    return FF_OK;
}

// 系统负载
FFResult platform_get_system_load(double* load1, double* load5, double* load15) {
    if (!load1 || !load5 || !load15) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    double load[3];
    if (getloadavg(load, 3) == 3) {
        *load1 = load[0];
        *load5 = load[1];
        *load15 = load[2];
        return FF_OK;
    }
    
    return FF_ERROR_UNKNOWN;
}

// 电池状态
FFResult platform_get_battery_info(int* percentage, bool* is_charging) {
    if (!percentage || !is_charging) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // FreeBSD 电池状态获取（如果适用）
    *percentage = 100;
    *is_charging = true;
    
    return FF_OK;
}
