#ifndef BRIDGE_H
#define BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

// 桥接层初始化
void bridge_init(void);

// 桥接层清理
void bridge_cleanup(void);

// 调用平台特定功能
int bridge_call_platform_function(const char* function_name, void* params, void* result);

// 注册平台回调
void bridge_register_callback(const char* callback_name, void (*callback)(void*));

// 触发平台回调
void bridge_trigger_callback(const char* callback_name, void* data);

#ifdef __cplusplus
}
#endif

#endif // BRIDGE_H