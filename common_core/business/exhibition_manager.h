#ifndef EXHIBITION_MANAGER_H
#define EXHIBITION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 展商回调函数类型
typedef void (*ExhibitionCallback)(void* user_data, int status, const void* data);

// 展商结构体
typedef struct {
    char* exhibition_id;
    char* name;
    char* description;
    char* url;
    char* start_time;
    char* end_time;
    char* location;
    int booth_count;
    bool is_active;
} Exhibition;

// 展商管理器结构体
typedef struct {
    void* network_manager;
    Exhibition* exhibitions;
    size_t exhibition_count;
} ExhibitionManager;

// 初始化展商管理器
ExhibitionManager* exhibition_manager_init();

// 销毁展商管理器
void exhibition_manager_destroy(ExhibitionManager* manager);

// 获取展商列表
bool exhibition_manager_get_exhibitions(ExhibitionManager* manager, ExhibitionCallback callback, void* user_data);

// 获取展商详情
bool exhibition_manager_get_exhibition_detail(ExhibitionManager* manager, const char* exhibition_id, ExhibitionCallback callback, void* user_data);

// 创建展商计划
bool exhibition_manager_create_exhibition(ExhibitionManager* manager, Exhibition* exhibition, ExhibitionCallback callback, void* user_data);

// 更新展商计划
bool exhibition_manager_update_exhibition(ExhibitionManager* manager, Exhibition* exhibition, ExhibitionCallback callback, void* user_data);

// 删除展商计划
bool exhibition_manager_delete_exhibition(ExhibitionManager* manager, const char* exhibition_id, ExhibitionCallback callback, void* user_data);

// 搜索展商
bool exhibition_manager_search_exhibition(ExhibitionManager* manager, const char* keyword, ExhibitionCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // EXHIBITION_MANAGER_H