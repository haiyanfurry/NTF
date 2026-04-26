// ====================================================================
// FindFriend 核心接口定义
// 所有模块通过这些接口进行交互
// 禁止跨模块直接调用内部函数
// ====================================================================

#ifndef FF_CORE_INTERFACE_H
#define FF_CORE_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ====================================================================
// 统一错误码
// ====================================================================
typedef enum FFResult {
    FF_OK = 0,
    FF_ERROR_INVALID_PARAM = -1,
    FF_ERROR_NO_MEMORY = -2,
    FF_ERROR_NOT_FOUND = -3,
    FF_ERROR_PERMISSION = -4,
    FF_ERROR_TIMEOUT = -5,
    FF_ERROR_NETWORK = -6,
    FF_ERROR_SECURITY = -7,
    FF_ERROR_NOT_INITIALIZED = -8,
    FF_ERROR_ALREADY_INITIALIZED = -9,
    FF_ERROR_UNSUPPORTED = -10,
    FF_ERROR_UNKNOWN = -99
} FFResult;

// ====================================================================
// 回调类型定义
// ====================================================================
typedef void (*FFAsyncCallback)(FFResult result, void* user_data);

typedef struct {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    const char* build_info;
} FFVersion;

// ====================================================================
// 核心初始化接口
// ====================================================================
FFResult ff_core_init(void);
FFResult ff_core_destroy(void);
bool ff_core_is_initialized(void);
FFVersion ff_core_get_version(void);
const char* ff_core_get_error_message(FFResult code);

// ====================================================================
// 模块初始化接口
// 每个模块必须实现这些函数
// ====================================================================
typedef FFResult (*ModuleInitFunc)(void);
typedef void (*ModuleCleanupFunc)(void);
typedef const char* (*ModuleGetNameFunc)(void);
typedef FFVersion (*ModuleGetVersionFunc)(void);

typedef struct FFModuleDescriptor {
    const char* name;
    ModuleInitFunc init;
    ModuleCleanupFunc cleanup;
    ModuleGetNameFunc get_name;
    ModuleGetVersionFunc get_version;
    uint32_t priority;  // 初始化优先级
} FFModuleDescriptor;

// 模块注册接口
FFResult ff_module_register(const FFModuleDescriptor* module);
FFResult ff_module_unregister(const char* module_name);
FFResult ff_module_init_all(void);
void ff_module_cleanup_all(void);

// ====================================================================
// 事件系统接口
// ====================================================================
typedef enum FFEventType {
    FF_EVENT_USER_LOGIN = 1,
    FF_EVENT_USER_LOGOUT = 2,
    FF_EVENT_MESSAGE_RECEIVED = 3,
    FF_EVENT_LOCATION_UPDATED = 4,
    FF_EVENT_FRIEND_ADDED = 5,
    FF_EVENT_FRIEND_REMOVED = 6,
    FF_EVENT_NETWORK_CHANGED = 7,
    FF_EVENT_SECURITY_ALERT = 8
} FFEventType;

typedef void (*FFEventCallback)(FFEventType event_type, const void* event_data, void* user_data);

FFResult ff_event_subscribe(FFEventType event_type, FFEventCallback callback, void* user_data);
FFResult ff_event_unsubscribe(FFEventType event_type, FFEventCallback callback);
FFResult ff_event_publish(FFEventType event_type, const void* event_data);

// ====================================================================
// 异步任务接口
// ====================================================================
typedef void* FFTaskHandle;

typedef enum FFTaskPriority {
    FF_TASK_PRIORITY_LOW = 0,
    FF_TASK_PRIORITY_NORMAL = 1,
    FF_TASK_PRIORITY_HIGH = 2
} FFTaskPriority;

FFTaskHandle ff_task_create(FFAsyncCallback callback, void* user_data);
FFResult ff_task_set_priority(FFTaskHandle handle, FFTaskPriority priority);
FFResult ff_task_set_timeout(FFTaskHandle handle, uint32_t timeout_ms);
FFResult ff_task_execute(FFTaskHandle handle);
FFResult ff_task_cancel(FFTaskHandle handle);
void ff_task_wait(FFTaskHandle handle);

// ====================================================================
// 配置接口
// ====================================================================
typedef struct FFConfig {
    const char* server_host;
    uint16_t server_port;
    bool enable_tls;
    uint32_t connection_timeout_ms;
    uint32_t max_retry_count;
    const char* log_level;
    const char* data_dir;
} FFConfig;

FFResult ff_config_load(const char* config_file);
FFResult ff_config_save(const char* config_file);
FFResult ff_config_get(const char* key, char* value, size_t value_size);
FFResult ff_config_set(const char* key, const char* value);
FFConfig* ff_config_get_current(void);

// ====================================================================
// 日志接口
// ====================================================================
typedef enum FFLogLevel {
    FF_LOG_DEBUG = 0,
    FF_LOG_INFO = 1,
    FF_LOG_WARN = 2,
    FF_LOG_ERROR = 3
} FFLogLevel;

void ff_log_set_level(FFLogLevel level);
void ff_log(FFLogLevel level, const char* module, const char* fmt, ...);

#define FF_LOG_DEBUG(fmt, ...) ff_log(FF_LOG_DEBUG, __FILE__, fmt, ##__VA_ARGS__)
#define FF_LOG_INFO(fmt, ...) ff_log(FF_LOG_INFO, __FILE__, fmt, ##__VA_ARGS__)
#define FF_LOG_WARN(fmt, ...) ff_log(FF_LOG_WARN, __FILE__, fmt, ##__VA_ARGS__)
#define FF_LOG_ERROR(fmt, ...) ff_log(FF_LOG_ERROR, __FILE__, fmt, ##__VA_ARGS__)

// ====================================================================
// Mock 接口（用于测试）
// ====================================================================
#ifdef FF_BUILD_TEST_MODE

typedef struct FFMockInterface {
    FFResult (*mock_init)(void);
    FFResult (*mock_operation)(int operation_id, void* param, void* result);
    FFResult (*mock_verify)(void);
} FFMockInterface;

FFResult ff_mock_register(const char* module_name, const FFMockInterface* mock);
FFResult ff_mock_unregister(const char* module_name);
FFResult ff_mock_inject_failure(const char* module_name, FFResult error_code);

#endif // FF_BUILD_TEST_MODE

#endif // FF_CORE_INTERFACE_H
