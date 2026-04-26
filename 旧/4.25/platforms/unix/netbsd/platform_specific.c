#define _GNU_SOURCE

#include "platform_specific.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int platform_mutex_init(platform_mutex_t* mutex) {
    if (!mutex) return -1;
    return pthread_mutex_init(mutex, NULL);
}

int platform_mutex_lock(platform_mutex_t* mutex) {
    if (!mutex) return -1;
    return pthread_mutex_lock(mutex);
}

int platform_mutex_unlock(platform_mutex_t* mutex) {
    if (!mutex) return -1;
    return pthread_mutex_unlock(mutex);
}

int platform_mutex_destroy(platform_mutex_t* mutex) {
    if (!mutex) return -1;
    return pthread_mutex_destroy(mutex);
}

int platform_thread_create(platform_thread_t* thread, platform_thread_func_t func, void* arg) {
    if (!thread || !func) return -1;
    return pthread_create(thread, NULL, func, arg);
}

int platform_thread_join(platform_thread_t thread, void** retval) {
    return pthread_join(thread, retval);
}

int platform_get_cpu_count(void) {
#if IS_BSD && defined(__NetBSD__)
    int ncpu;
    size_t len = sizeof(ncpu);
    if (sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0) == 0) {
        return ncpu;
    }
#endif
    return 1;
}

int64_t platform_get_memory_size(void) {
#if IS_BSD && defined(__NetBSD__)
    int64_t memsize;
    size_t len = sizeof(memsize);
    if (sysctlbyname("hw.physmem", &memsize, &len, NULL, 0) == 0) {
        return memsize;
    }
#endif
    return 0;
}

int64_t platform_get_free_memory(void) {
#if IS_BSD && defined(__NetBSD__)
    u_int pagesize;
    size_t len = sizeof(pagesize);
    if (sysctlbyname("hw.pagesize", &pagesize, &len, NULL, 0) != 0) {
        return 0;
    }

    struct vmmeter vm;
    len = sizeof(vm);
    if (sysctlbyname("vm.stats.vm", &vm, &len, NULL, 0) != 0) {
        return 0;
    }

    return (int64_t)(pagesize * vm.freetarg);
#else
    return 0;
#endif
}

const char* platform_get_hostname(void) {
    static char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return hostname;
    }
    return "unknown";
}

int platform_set_hostname(const char* new_hostname) {
    if (!new_hostname) return -1;
    return sethostname(new_hostname, strlen(new_hostname));
}

const char* platform_get_username(void) {
#if IS_BSD
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return pw->pw_name;
    }
#endif
    return "unknown";
}

const char* platform_get_home_directory(void) {
#if IS_BSD
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return pw->pw_dir;
    }
#endif
    return "/tmp";
}

int platform_get_system_uptime(void) {
#if IS_BSD && defined(__NetBSD__)
    struct timeval boottime;
    size_t len = sizeof(boottime);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};

    if (sysctl(mib, 2, &boottime, &len, NULL, 0) == 0) {
        struct timeval now;
        gettimeofday(&now, NULL);
        return (int)(now.tv_sec - boottime.tv_sec);
    }
#endif
    return 0;
}

int platform_is_root(void) {
    return getuid() == 0;
}

int platform_drop_privileges(const char* user) {
#if IS_BSD
    if (!user || !platform_is_root()) return -1;

    struct passwd* pw = getpwnam(user);
    if (!pw) return -1;

    if (initgroups(pw->pw_name, pw->pw_gid) != 0) return -1;
    if (setgid(pw->pw_gid) != 0) return -1;
    if (setuid(pw->pw_uid) != 0) return -1;

    return 0;
#else
    return -1;
#endif
}

char* platform_get_real_path(const char* path, char* resolved, size_t len) {
    if (!path || !resolved) return NULL;
    return realpath(path, resolved);
}

int platform_check_file_permissions(const char* path) {
    if (!path) return -1;

    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }

    if (st.st_uid == getuid()) {
        return (st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    }

    return (st.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH));
}

int platform_init_network(void) {
    return 0;
}

int platform_cleanup_network(void) {
    return 0;
}

int platform_get_network_interfaces(char* buffer, size_t len) {
    if (!buffer || len == 0) return -1;

    FILE* fp = popen("ifconfig -a 2>/dev/null | grep 'inet ' | awk '{print $2}'", "r");
    if (!fp) {
        snprintf(buffer, len, "localhost");
        return 0;
    }

    char line[256];
    size_t offset = 0;
    buffer[0] = '\0';

    while (fgets(line, sizeof(line), fp) != NULL && offset < len - 1) {
        size_t linelen = strlen(line);
        if (offset + linelen < len) {
            strcpy(buffer + offset, line);
            offset += linelen;
        }
    }

    pclose(fp);

    if (offset == 0) {
        snprintf(buffer, len, "localhost");
    }

    return 0;
}

void platform_log_system_info(void) {
    printf("[PLATFORM] System Info for %s:\n", PLATFORM_FULL_NAME);
    printf("  Hostname: %s\n", platform_get_hostname());
    printf("  Username: %s\n", platform_get_username());
    printf("  Home: %s\n", platform_get_home_directory());
    printf("  CPU Count: %d\n", platform_get_cpu_count());
    printf("  Memory: %ld MB total\n", (long)(platform_get_memory_size() / 1024 / 1024));
    printf("  Free Memory: %ld MB\n", (long)(platform_get_free_memory() / 1024 / 1024));
    printf("  Uptime: %d seconds\n", platform_get_system_uptime());
    printf("  Is Root: %s\n", platform_is_root() ? "YES" : "NO");
}

const char* platform_get_platform_name(void) {
    return PLATFORM_FULL_NAME;
}

int platform_random_bytes(void* buffer, size_t len) {
    if (!buffer) return -1;

    FILE* fp = fopen("/dev/urandom", "rb");
    if (!fp) {
        srand(time(NULL));
        unsigned char* buf = (unsigned char*)buffer;
        for (size_t i = 0; i < len; i++) {
            buf[i] = rand() % 256;
        }
        return 0;
    }

    size_t ret = fread(buffer, 1, len, fp);
    fclose(fp);

    return (ret == len) ? 0 : -1;
}
