// FindFriend Linux 平台特定实现

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

FFResult ff_platform_init(void) {
    FF_LOG_INFO("Linux platform initialized");
    return FF_OK;
}

void ff_platform_cleanup(void) {
    FF_LOG_INFO("Linux platform cleanup");
}

FFResult ff_platform_get_info(FFPlatformInfo* info) {
    if (!info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct utsname uname_info;
    if (uname(&uname_info) != 0) {
        return FF_ERROR_UNKNOWN;
    }
    
    strncpy(info->os_name, "Linux", sizeof(info->os_name) - 1);
    strncpy(info->os_version, uname_info.release, sizeof(info->os_version) - 1);
    strncpy(info->hostname, uname_info.nodename, sizeof(info->hostname) - 1);
    strncpy(info->arch, uname_info.machine, sizeof(info->arch) - 1);
    
    info->pid = getpid();
    info->uid = getuid();
    info->gid = getgid();
    
    return FF_OK;
}

FFResult ff_platform_get_cpu_info(FFCPUInfo* info) {
    if (!info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        return FF_ERROR_UNKNOWN;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                strncpy(info->model, colon + 2, sizeof(info->model) - 1);
                // 去除换行符
                size_t len = strlen(info->model);
                if (len > 0 && info->model[len - 1] == '\n') {
                    info->model[len - 1] = '\0';
                }
            }
        } else if (strncmp(line, "cpu cores", 9) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                info->cores = atoi(colon + 2);
            }
        }
    }
    
    fclose(fp);
    return FF_OK;
}

FFResult ff_platform_get_memory_info(FFMemoryInfo* info) {
    if (!info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return FF_ERROR_UNKNOWN;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal", 8) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                info->total = atoi(colon + 2) * 1024; // 转换为字节
            }
        } else if (strncmp(line, "MemFree", 7) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                info->free = atoi(colon + 2) * 1024; // 转换为字节
            }
        }
    }
    
    fclose(fp);
    return FF_OK;
}

FFResult ff_platform_get_disk_info(FFDiskInfo* info) {
    if (!info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct statvfs stat;
    if (statvfs(".", &stat) != 0) {
        return FF_ERROR_UNKNOWN;
    }
    
    info->total = stat.f_blocks * stat.f_frsize;
    info->free = stat.f_bavail * stat.f_frsize;
    info->used = info->total - info->free;
    
    return FF_OK;
}

FFResult ff_platform_get_network_info(FFNetworkInfo* info) {
    if (!info) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，实际应该解析 /proc/net/dev
    strncpy(info->hostname, "localhost", sizeof(info->hostname) - 1);
    strncpy(info->ip_address, "127.0.0.1", sizeof(info->ip_address) - 1);
    
    return FF_OK;
}

FFResult ff_platform_execute(const char* command, char* output, size_t output_size) {
    if (!command || !output) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FILE* fp = popen(command, "r");
    if (!fp) {
        return FF_ERROR_UNKNOWN;
    }
    
    size_t bytes_read = fread(output, 1, output_size - 1, fp);
    output[bytes_read] = '\0';
    
    pclose(fp);
    return FF_OK;
}

FFResult ff_platform_create_directory(const char* path) {
    if (!path) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (mkdir(path, 0755) != 0 && errno != EEXIST) {
        return FF_ERROR_UNKNOWN;
    }
    
    return FF_OK;
}

FFResult ff_platform_delete_file(const char* path) {
    if (!path) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (unlink(path) != 0) {
        return FF_ERROR_UNKNOWN;
    }
    
    return FF_OK;
}

FFResult ff_platform_get_file_size(const char* path, size_t* size) {
    if (!path || !size) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct stat stat;
    if (stat(path, &stat) != 0) {
        return FF_ERROR_UNKNOWN;
    }
    
    *size = stat.st_size;
    return FF_OK;
}

FFResult ff_platform_is_file_exists(const char* path, bool* exists) {
    if (!path || !exists) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct stat stat;
    *exists = (stat(path, &stat) == 0);
    return FF_OK;
}

FFResult ff_platform_sleep(uint32_t milliseconds) {
    usleep(milliseconds * 1000);
    return FF_OK;
}

FFResult ff_platform_get_current_time(uint64_t* timestamp) {
    if (!timestamp) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *timestamp = time(NULL);
    return FF_OK;
}

FFResult ff_platform_generate_random(uint8_t* buffer, size_t length) {
    if (!buffer) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    FILE* fp = fopen("/dev/urandom", "r");
    if (!fp) {
        return FF_ERROR_UNKNOWN;
    }
    
    size_t bytes_read = fread(buffer, 1, length, fp);
    fclose(fp);
    
    if (bytes_read != length) {
        return FF_ERROR_UNKNOWN;
    }
    
    return FF_OK;
}
