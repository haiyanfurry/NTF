// ====================================================================
// FindFriend 游戏/互动接口
// 业务模块：游戏管理、互动功能
// ====================================================================

#ifndef FF_GAME_INTERFACE_H
#define FF_GAME_INTERFACE_H

#include "ff_core_interface.h"
#include <stdint.h>

// ====================================================================
// 游戏状态
// ====================================================================
typedef enum FFGameStatus {
    FF_GAME_WAITING = 0,
    FF_GAME_RUNNING = 1,
    FF_GAME_PAUSED = 2,
    FF_GAME_FINISHED = 3
} FFGameStatus;

typedef enum FFGameType {
    FF_GAME_CHAT = 1,
    FF_GAME_QUIZ = 2,
    FF_GAME_LOCATION = 3,
    FF_GAME_AR = 4
} FFGameType;

// ====================================================================
// 游戏数据结构
// ====================================================================
typedef struct FFGame {
    uint64_t game_id;
    FFGameType type;
    char name[128];
    char description[512];
    uint32_t creator_uid;
    FFGameStatus status;
    uint32_t max_players;
    uint32_t current_players;
    uint64_t created_at;
    uint64_t started_at;
} FFGame;

typedef struct FFGamePlayer {
    uint32_t uid;
    char username[64];
    uint32_t score;
    uint8_t rank;
    bool is_ready;
    uint64_t joined_at;
} FFGamePlayer;

// ====================================================================
// 游戏动作
// ====================================================================
typedef struct FFGameAction {
    uint64_t game_id;
    uint32_t player_uid;
    char action_type[32];
    char action_data[1024];
    uint64_t timestamp;
} FFGameAction;

// ====================================================================
// 游戏管理接口
// ====================================================================

// 游戏列表
FFResult ff_game_get_list(FFGame** games, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_game_get_by_type(FFGameType type, FFGame** games, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_game_get_detail(uint64_t game_id, FFGame** game, FFAsyncCallback callback, void* user_data);

// 游戏创建
FFResult ff_game_create(FFGameType type, const char* name, const char* description, uint32_t max_players, uint64_t* game_id, FFAsyncCallback callback, void* user_data);
FFResult ff_game_delete(uint64_t game_id, FFAsyncCallback callback, void* user_data);

// 游戏参与
FFResult ff_game_join(uint64_t game_id, FFAsyncCallback callback, void* user_data);
FFResult ff_game_leave(uint64_t game_id, FFAsyncCallback callback, void* user_data);
FFResult ff_game_get_players(uint64_t game_id, FFGamePlayer** players, size_t* count, FFAsyncCallback callback, void* user_data);

// 游戏动作
FFResult ff_game_action(uint64_t game_id, const char* action_type, const char* action_data, FFAsyncCallback callback, void* user_data);
FFResult ff_game_get_actions(uint64_t game_id, FFGameAction** actions, size_t* count, FFAsyncCallback callback, void* user_data);

// 游戏状态
FFResult ff_game_start(uint64_t game_id, FFAsyncCallback callback, void* user_data);
FFResult ff_game_pause(uint64_t game_id, FFAsyncCallback callback, void* user_data);
FFResult ff_game_resume(uint64_t game_id, FFAsyncCallback callback, void* user_data);
FFResult ff_game_end(uint64_t game_id, FFAsyncCallback callback, void* user_data);

// 计分和排名
FFResult ff_game_update_score(uint64_t game_id, uint32_t player_uid, uint32_t score, FFAsyncCallback callback, void* user_data);
FFResult ff_game_get_leaderboard(uint64_t game_id, FFGamePlayer** leaderboard, size_t* count, FFAsyncCallback callback, void* user_data);

// 内存释放
void ff_game_free(FFGame* game);
void ff_game_list_free(FFGame* games, size_t count);
void ff_game_player_list_free(FFGamePlayer* players, size_t count);
void ff_game_action_list_free(FFGameAction* actions, size_t count);

#endif // FF_GAME_INTERFACE_H
