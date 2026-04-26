#ifndef PLATFORM_SPECIFIC_H
#define PLATFORM_SPECIFIC_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#ifdef __OpenBSD__
#include <sys/sysctl.h>
#include <pwd.h>
#include <grp.h>
#define PLATFORM_NAME "OpenBSD"
#define PLATFORM_FULL_NAME "OpenBSD Unix"
#define IS_BSD 1
#elif defined(__FreeBSD__)
#include <sys/sysctl.h>
#include <pwd.h>
#include <grp.h>
#define PLATFORM_NAME "FreeBSD"
#define PLATFORM_FULL_NAME "FreeBSD Unix"
#define IS_BSD 1
#elif defined(__NetBSD__)
#include <sys/sysctl.h>
#include <pwd.h>
#include <grp.h>
#define PLATFORM_NAME "NetBSD"
#define PLATFORM_FULL_NAME "NetBSD Unix"
#define IS_BSD 1
#else
#define PLATFORM_NAME "Unknown BSD"
#define PLATFORM_FULL_NAME "Unknown BSD Unix (Simulated)"
#define IS_BSD 0
#endif

typedef pthread_mutex_t platform_mutex_t;

#define PLATFORM_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

int platform_mutex_init(platform_mutex_t* mutex);
int platform_mutex_lock(platform_mutex_t* mutex);
int platform_mutex_unlock(platform_mutex_t* mutex);
int platform_mutex_destroy(platform_mutex_t* mutex);

typedef pthread_t platform_thread_t;
typedef void* (*platform_thread_func_t)(void*);

int platform_thread_create(platform_thread_t* thread, platform_thread_func_t func, void* arg);
int platform_thread_join(platform_thread_t thread, void** retval);

int platform_get_cpu_count(void);
int64_t platform_get_memory_size(void);
int64_t platform_get_free_memory(void);

const char* platform_get_hostname(void);
int platform_set_hostname(const char* hostname);

const char* platform_get_username(void);
const char* platform_get_home_directory(void);

int platform_get_system_uptime(void);

int platform_is_root(void);
int platform_drop_privileges(const char* user);

char* platform_get_real_path(const char* path, char* resolved, size_t len);
int platform_check_file_permissions(const char* path);

int platform_init_network(void);
int platform_cleanup_network(void);

int platform_get_network_interfaces(char* buffer, size_t len);

void platform_log_system_info(void);
const char* platform_get_platform_name(void);

int platform_random_bytes(void* buffer, size_t len);

#endif // PLATFORM_SPECIFIC_H
