#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// 游戏结构体
typedef struct {
    char game_id[36];           // 游戏 ID
    char name[100];             // 游戏名称
    char description[500];       // 游戏描述
    char icon_url[256];          // 图标 URL
    char version[20];            // 游戏版本
    int status;                  // 游戏状态
    int required_permission;     // 所需权限
} Game;

// 游戏数据结构体
typedef struct {
    char game_id[36];           // 游戏 ID
    char user_id[36];            // 用户 ID
    char data[1024];             // 游戏数据
    time_t last_sync;            // 最后同步时间
    int sync_status;             // 同步状态
} GameData;

// 游戏回调函数类型
typedef void (*GameCallback)(void* user_data, int status, const uint8_t* data, size_t length);

// 游戏管理器结构体
typedef struct {
    Game* games;                 // 游戏列表
    int game_count;              // 游戏数量
    GameData* game_data;         // 游戏数据列表
    int game_data_count;         // 游戏数据数量
    bool initialized;            // 初始化状态
} GameManager;

// 初始化游戏管理器
GameManager* game_manager_init(void);

// 销毁游戏管理器
void game_manager_destroy(GameManager* manager);

// 获取游戏列表
bool game_manager_get_games(GameManager* manager, GameCallback callback, void* user_data);

// 获取游戏详情
bool game_manager_get_game_detail(GameManager* manager, const char* game_id, GameCallback callback, void* user_data);

// 检查游戏权限
bool game_manager_check_permission(GameManager* manager, const char* game_id, int permission, GameCallback callback, void* user_data);

// 同步游戏数据
bool game_manager_sync_game_data(GameManager* manager, const char* game_id, const char* data, GameCallback callback, void* user_data);

// 获取游戏数据
bool game_manager_get_game_data(GameManager* manager, const char* game_id, GameCallback callback, void* user_data);

// 绑定游戏账号
bool game_manager_bind_game_account(GameManager* manager, const char* game_id, const char* account_id, GameCallback callback, void* user_data);

// 推送游戏通知
bool game_manager_push_game_notification(GameManager* manager, const char* game_id, const char* message, GameCallback callback, void* user_data);

// 扩展游戏功能
bool game_manager_extend_game_function(GameManager* manager, const char* game_id, const char* function_name, const char* parameters, GameCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // GAME_MANAGER_H