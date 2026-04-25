#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 数据存储类型枚举
typedef enum {
    DATA_STORAGE_IN_MEMORY = 0,
    DATA_STORAGE_FILE = 1,
    DATA_STORAGE_DATABASE = 2
} DataStorageType;

// 数据操作结果
typedef enum {
    DATA_RESULT_SUCCESS = 0,
    DATA_RESULT_NOT_FOUND = 1,
    DATA_RESULT_DUPLICATE = 2,
    DATA_RESULT_WRITE_ERROR = 3,
    DATA_RESULT_READ_ERROR = 4,
    DATA_RESULT_INVALID_PARAM = 5,
    DATA_RESULT_MEMORY_ERROR = 6,
    DATA_RESULT_UNKNOWN = 7
} DataResult;

// 键值对结构体
typedef struct {
    char* key;
    void* value;
    size_t value_size;
    int value_type;
} KeyValuePair;

// 数据管理器结构体
typedef struct {
    DataStorageType storage_type;
    void* storage_context;
    KeyValuePair* items;
    size_t item_count;
    size_t item_capacity;
} DataManager;

// 数据回调函数类型
typedef void (*DataCallback)(void* user_data, DataResult result, void* data, size_t size);

// 初始化数据管理器
DataManager* data_manager_init(DataStorageType type, const char* storage_path);

// 销毁数据管理器
void data_manager_destroy(DataManager* manager);

// 保存数据
DataResult data_manager_save(DataManager* manager, const char* key, const void* value, size_t value_size);

// 获取数据
DataResult data_manager_get(DataManager* manager, const char* key, void** value, size_t* value_size);

// 删除数据
DataResult data_manager_delete(DataManager* manager, const char* key);

// 检查键是否存在
bool data_manager_has(DataManager* manager, const char* key);

// 清空所有数据
DataResult data_manager_clear(DataManager* manager);

// 获取所有键
char** data_manager_get_all_keys(DataManager* manager, size_t* count);

// 保存字符串
DataResult data_manager_save_string(DataManager* manager, const char* key, const char* value);

// 获取字符串
DataResult data_manager_get_string(DataManager* manager, const char* key, char** value);

// 保存整数
DataResult data_manager_save_int(DataManager* manager, const char* key, int value);

// 获取整数
DataResult data_manager_get_int(DataManager* manager, const char* key, int* value);

// 保存浮点数
DataResult data_manager_save_double(DataManager* manager, const char* key, double value);

// 获取浮点数
DataResult data_manager_get_double(DataManager* manager, const char* key, double* value);

// 保存布尔值
DataResult data_manager_save_bool(DataManager* manager, const char* key, bool value);

// 获取布尔值
DataResult data_manager_get_bool(DataManager* manager, const char* key, bool* value);

// 持久化到磁盘
DataResult data_manager_flush(DataManager* manager);

// 从磁盘加载
DataResult data_manager_load(DataManager* manager);

#ifdef __cplusplus
}
#endif

#endif // DATA_MANAGER_H
