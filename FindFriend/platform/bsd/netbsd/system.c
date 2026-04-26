// FindFriend NetBSD 系统接口实现

#include "platform_specific.h"
#include "../../../common_core/interface/ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <uvm/uvm_extern.h>
#include <pwd.h>
#include <sys/resource.h>
#include <time.h>

FFResult platform_get_system_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }

    char osname[256];
    char osversion[256];
    size_t len;

    len = sizeof(osname);
    sysctlbyname("kern.ostype", osname, &len, NULL, 0);

    len = sizeof(osversion);
    sysctlbyname("kern.osrelease", osversion, &len, NULL, 0);

    snprintf(buffer, buffer_size, "OS: %s %s\n", osname, osversion);

    return FF_OK;
}

FFResult platform_get_process_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }

    pid_t pid = getpid();
    struct passwd* pw = getpwuid(getuid());

    snprintf(buffer, buffer_size, "PID: %d\nUser: %s\n", pid, pw ? pw->pw_name : "unknown");

    return FF_OK;
}

FFResult platform_get_filesystem_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }

    snprintf(buffer, buffer_size, "Filesystem: FFS\n");

    return FF_OK;
}

FFResult platform_get_memory_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }

    uint64_t total_mem, free_mem;
    size_t len = sizeof(total_mem);

    sysctlbyname("hw.physmem", &total_mem, &len, NULL, 0);

    struct vmmeter vm;
    len = sizeof(vm);
    sysctlbyname("vm.vmmeter", &vm, &len, NULL, 0);

    free_mem = (uint64_t)vm.free_count * PAGE_SIZE;

    snprintf(buffer, buffer_size,
             "Total Memory: %llu MB\nFree Memory: %llu MB\n",
             (unsigned long long)(total_mem / 1024 / 1024),
             (unsigned long long)(free_mem / 1024 / 1024));

    return FF_OK;
}

FFResult platform_get_system_load(double* load1, double* load5, double* load15) {
    if (!load1 || !load5 || !load15) {
        return FF_ERROR_INVALID_PARAM;
    }

    struct loadavg loadavg;
    size_t len = sizeof(loadavg);

    if (sysctlbyname("vm.loadavg", &loadavg, &len, NULL, 0) == -1) {
        return FF_ERROR;
    }

    *load1 = (double)loadavg.ldavg[0] / loadavg.fscale;
    *load5 = (double)loadavg.ldavg[1] / loadavg.fscale;
    *load15 = (double)loadavg.ldavg[2] / loadavg.fscale;

    return FF_OK;
}

FFResult platform_get_battery_info(int* percentage, bool* is_charging) {
    if (!percentage || !is_charging) {
        return FF_ERROR_INVALID_PARAM;
    }

    *percentage = -1;
    *is_charging = false;

    return FF_OK;
}

FFResult platform_get_cpu_info(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }

    char cpu_model[256];
    size_t len = sizeof(cpu_model);

    sysctlbyname("hw.model", cpu_model, &len, NULL, 0);

    int cpu_count;
    len = sizeof(cpu_count);
    sysctlbyname("hw.ncpu", &cpu_count, &len, NULL, 0);

    snprintf(buffer, buffer_size, "CPU: %s (%d cores)\n", cpu_model, cpu_count);

    return FF_OK;
}

FFResult platform_get_uptime(uint64_t* uptime) {
    if (!uptime) {
        return FF_ERROR_INVALID_PARAM;
    }

    struct timeval boottime;
    size_t len = sizeof(boottime);
    time_t now = time(NULL);

    if (sysctlbyname("kern.boottime", &boottime, &len, NULL, 0) == -1) {
        return FF_ERROR;
    }

    *uptime = (uint64_t)(now - boottime.tv_sec);

    return FF_OK;
}
