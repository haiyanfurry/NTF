#include "bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 回调函数结构
typedef struct {
    char name[100];
    void (*callback)(void*);
} CallbackEntry;

// 回调列表
static CallbackEntry* callbacks = NULL;
static int callback_count = 0;

// 桥接层初始化
void bridge_init(void) {
    printf("Initializing bridge layer...\n");
    callbacks = malloc(sizeof(CallbackEntry) * 10);
    callback_count = 0;
    printf("Bridge layer initialized successfully\n");
}

// 桥接层清理
void bridge_cleanup(void) {
    printf("Cleaning up bridge layer...\n");
    if (callbacks) {
        free(callbacks);
        callbacks = NULL;
    }
    callback_count = 0;
    printf("Bridge layer cleaned up successfully\n");
}

// 调用平台特定功能
int bridge_call_platform_function(const char* function_name, void* params, void* result) {
    printf("Calling platform function: %s\n", function_name);
    // 这里应该根据不同平台调用相应的实现
    // 目前返回成功
    return 0;
}

// 注册平台回调
void bridge_register_callback(const char* callback_name, void (*callback)(void*)) {
    printf("Registering callback: %s\n", callback_name);
    if (callback_count >= 10) {
        callbacks = realloc(callbacks, sizeof(CallbackEntry) * (callback_count + 10));
    }
    strcpy(callbacks[callback_count].name, callback_name);
    callbacks[callback_count].callback = callback;
    callback_count++;
}

// 触发平台回调
void bridge_trigger_callback(const char* callback_name, void* data) {
    printf("Triggering callback: %s\n", callback_name);
    for (int i = 0; i < callback_count; i++) {
        if (strcmp(callbacks[i].name, callback_name) == 0) {
            callbacks[i].callback(data);
            return;
        }
    }
    printf("Callback not found: %s\n", callback_name);
}