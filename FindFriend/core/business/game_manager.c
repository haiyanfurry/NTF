// FindFriend 游戏管理模块

#include "ff_core_interface.h"
#include "ff_game_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_GAMES 128
#define MAX_PLAYERS 1024

typedef struct {
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
} Game;

typedef struct {
    uint32_t uid;
    uint64_t game_id;
    char username[64];
    uint32_t score;
    uint8_t rank;
    bool is_ready;
    uint64_t joined_at;
} GamePlayer;

static Game g_games[MAX_GAMES];
static size_t g_game_count = 0;
static GamePlayer g_players[MAX_PLAYERS];
static size_t g_player_count = 0;
static uint64_t g_next_game_id = 10000;

FFResult ff_game_init(void) {
    g_game_count = 0;
    g_player_count = 0;
    g_next_game_id = 10000;
    memset(g_games, 0, sizeof(g_games));
    memset(g_players, 0, sizeof(g_players));
    FF_LOG_INFO("Game manager initialized");
    return FF_OK;
}

void ff_game_cleanup(void) {
    g_game_count = 0;
    g_player_count = 0;
    FF_LOG_INFO("Game manager cleanup");
}

FFResult ff_game_get_list(FFGame** games, size_t* count, FFAsyncCallback callback, void* user_data) {
    if (!games || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_game_count == 0) {
        *games = NULL;
        *count = 0;
        if (callback) {
            callback(FF_OK, user_data);
        }
        return FF_OK;
    }
    
    *games = (FFGame*)malloc(g_game_count * sizeof(FFGame));
    if (!*games) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    for (size_t i = 0; i < g_game_count; i++) {
        Game* game = &g_games[i];
        (*games)[i].game_id = game->game_id;
        (*games)[i].type = game->type;
        strncpy((*games)[i].name, game->name, sizeof((*games)[i].name) - 1);
        strncpy((*games)[i].description, game->description, sizeof((*games)[i].description) - 1);
        (*games)[i].creator_uid = game->creator_uid;
        (*games)[i].status = game->status;
        (*games)[i].max_players = game->max_players;
        (*games)[i].current_players = game->current_players;
        (*games)[i].created_at = game->created_at;
        (*games)[i].started_at = game->started_at;
    }
    
    *count = g_game_count;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_get_by_type(FFGameType type, FFGame** games, size_t* count, FFAsyncCallback callback, void* user_data) {
    if (!games || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t match_count = 0;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].type == type) {
            match_count++;
        }
    }
    
    if (match_count == 0) {
        *games = NULL;
        *count = 0;
        if (callback) {
            callback(FF_OK, user_data);
        }
        return FF_OK;
    }
    
    *games = (FFGame*)malloc(match_count * sizeof(FFGame));
    if (!*games) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    size_t index = 0;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].type == type) {
            Game* game = &g_games[i];
            (*games)[index].game_id = game->game_id;
            (*games)[index].type = game->type;
            strncpy((*games)[index].name, game->name, sizeof((*games)[index].name) - 1);
            strncpy((*games)[index].description, game->description, sizeof((*games)[index].description) - 1);
            (*games)[index].creator_uid = game->creator_uid;
            (*games)[index].status = game->status;
            (*games)[index].max_players = game->max_players;
            (*games)[index].current_players = game->current_players;
            (*games)[index].created_at = game->created_at;
            (*games)[index].started_at = game->started_at;
            index++;
        }
    }
    
    *count = match_count;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_get_detail(uint64_t game_id, FFGame** game, FFAsyncCallback callback, void* user_data) {
    if (!game) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            Game* g_game = &g_games[i];
            *game = (FFGame*)malloc(sizeof(FFGame));
            if (!*game) {
                if (callback) {
                    callback(FF_ERROR_NO_MEMORY, user_data);
                }
                return FF_ERROR_NO_MEMORY;
            }
            
            (*game)->game_id = g_game->game_id;
            (*game)->type = g_game->type;
            strncpy((*game)->name, g_game->name, sizeof((*game)->name) - 1);
            strncpy((*game)->description, g_game->description, sizeof((*game)->description) - 1);
            (*game)->creator_uid = g_game->creator_uid;
            (*game)->status = g_game->status;
            (*game)->max_players = g_game->max_players;
            (*game)->current_players = g_game->current_players;
            (*game)->created_at = g_game->created_at;
            (*game)->started_at = g_game->started_at;
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_game_create(FFGameType type, const char* name, const char* description, uint32_t max_players, uint64_t* game_id, FFAsyncCallback callback, void* user_data) {
    if (!name || !game_id) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (g_game_count >= MAX_GAMES) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    Game* game = &g_games[g_game_count];
    game->game_id = g_next_game_id++;
    game->type = type;
    strncpy(game->name, name, sizeof(game->name) - 1);
    if (description) {
        strncpy(game->description, description, sizeof(game->description) - 1);
    }
    game->creator_uid = 1000; // 临时值
    game->status = FF_GAME_WAITING;
    game->max_players = max_players;
    game->current_players = 0;
    game->created_at = time(NULL);
    game->started_at = 0;
    
    g_game_count++;
    *game_id = game->game_id;
    
    FF_LOG_INFO("Game created: %s (ID: %llu) type: %d", name, game->game_id, type);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_delete(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    for (size_t i = 0; i < g_game_count; ) {
        if (g_games[i].game_id == game_id) {
            // 移除相关玩家
            for (size_t j = 0; j < g_player_count; ) {
                if (g_players[j].game_id == game_id) {
                    memmove(&g_players[j], &g_players[j + 1], (g_player_count - j - 1) * sizeof(GamePlayer));
                    g_player_count--;
                } else {
                    j++;
                }
            }
            
            // 移除游戏
            memmove(&g_games[i], &g_games[i + 1], (g_game_count - i - 1) * sizeof(Game));
            g_game_count--;
            FF_LOG_INFO("Game deleted: ID %llu", game_id);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        } else {
            i++;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_game_join(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    Game* game = NULL;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            game = &g_games[i];
            break;
        }
    }
    
    if (!game) {
        if (callback) {
            callback(FF_ERROR_NOT_FOUND, user_data);
        }
        return FF_ERROR_NOT_FOUND;
    }
    
    if (game->status != FF_GAME_WAITING) {
        if (callback) {
            callback(FF_ERROR_UNKNOWN, user_data);
        }
        return FF_ERROR_UNKNOWN;
    }
    
    if (game->current_players >= game->max_players) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    if (g_player_count >= MAX_PLAYERS) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    GamePlayer* player = &g_players[g_player_count];
    player->uid = 1000; // 临时值
    player->game_id = game_id;
    strncpy(player->username, "user", sizeof(player->username) - 1);
    player->score = 0;
    player->rank = 0;
    player->is_ready = false;
    player->joined_at = time(NULL);
    
    g_player_count++;
    game->current_players++;
    
    FF_LOG_INFO("User joined game %llu", game_id);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_leave(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    Game* game = NULL;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            game = &g_games[i];
            break;
        }
    }
    
    if (!game) {
        if (callback) {
            callback(FF_ERROR_NOT_FOUND, user_data);
        }
        return FF_ERROR_NOT_FOUND;
    }
    
    for (size_t i = 0; i < g_player_count; ) {
        if (g_players[i].game_id == game_id) {
            memmove(&g_players[i], &g_players[i + 1], (g_player_count - i - 1) * sizeof(GamePlayer));
            g_player_count--;
            game->current_players--;
            FF_LOG_INFO("User left game %llu", game_id);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        } else {
            i++;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_game_get_players(uint64_t game_id, FFGamePlayer** players, size_t* count, FFAsyncCallback callback, void* user_data) {
    if (!players || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t player_count = 0;
    for (size_t i = 0; i < g_player_count; i++) {
        if (g_players[i].game_id == game_id) {
            player_count++;
        }
    }
    
    if (player_count == 0) {
        *players = NULL;
        *count = 0;
        if (callback) {
            callback(FF_OK, user_data);
        }
        return FF_OK;
    }
    
    *players = (FFGamePlayer*)malloc(player_count * sizeof(FFGamePlayer));
    if (!*players) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    size_t index = 0;
    for (size_t i = 0; i < g_player_count; i++) {
        if (g_players[i].game_id == game_id) {
            GamePlayer* p = &g_players[i];
            (*players)[index].uid = p->uid;
            strncpy((*players)[index].username, p->username, sizeof((*players)[index].username) - 1);
            (*players)[index].score = p->score;
            (*players)[index].rank = p->rank;
            (*players)[index].is_ready = p->is_ready;
            (*players)[index].joined_at = p->joined_at;
            index++;
        }
    }
    
    *count = player_count;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_action(uint64_t game_id, const char* action_type, const char* action_data, FFAsyncCallback callback, void* user_data) {
    FF_LOG_INFO("Game action: %s on game %llu", action_type, game_id);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_get_actions(uint64_t game_id, FFGameAction** actions, size_t* count, FFAsyncCallback callback, void* user_data) {
    *actions = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_start(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    Game* game = NULL;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            game = &g_games[i];
            break;
        }
    }
    
    if (!game) {
        if (callback) {
            callback(FF_ERROR_NOT_FOUND, user_data);
        }
        return FF_ERROR_NOT_FOUND;
    }
    
    if (game->status != FF_GAME_WAITING) {
        if (callback) {
            callback(FF_ERROR_UNKNOWN, user_data);
        }
        return FF_ERROR_UNKNOWN;
    }
    
    game->status = FF_GAME_RUNNING;
    game->started_at = time(NULL);
    
    FF_LOG_INFO("Game started: %llu", game_id);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_pause(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    Game* game = NULL;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            game = &g_games[i];
            break;
        }
    }
    
    if (!game) {
        if (callback) {
            callback(FF_ERROR_NOT_FOUND, user_data);
        }
        return FF_ERROR_NOT_FOUND;
    }
    
    if (game->status != FF_GAME_RUNNING) {
        if (callback) {
            callback(FF_ERROR_UNKNOWN, user_data);
        }
        return FF_ERROR_UNKNOWN;
    }
    
    game->status = FF_GAME_PAUSED;
    FF_LOG_INFO("Game paused: %llu", game_id);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_resume(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    Game* game = NULL;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            game = &g_games[i];
            break;
        }
    }
    
    if (!game) {
        if (callback) {
            callback(FF_ERROR_NOT_FOUND, user_data);
        }
        return FF_ERROR_NOT_FOUND;
    }
    
    if (game->status != FF_GAME_PAUSED) {
        if (callback) {
            callback(FF_ERROR_UNKNOWN, user_data);
        }
        return FF_ERROR_UNKNOWN;
    }
    
    game->status = FF_GAME_RUNNING;
    FF_LOG_INFO("Game resumed: %llu", game_id);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_end(uint64_t game_id, FFAsyncCallback callback, void* user_data) {
    Game* game = NULL;
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            game = &g_games[i];
            break;
        }
    }
    
    if (!game) {
        if (callback) {
            callback(FF_ERROR_NOT_FOUND, user_data);
        }
        return FF_ERROR_NOT_FOUND;
    }
    
    if (game->status != FF_GAME_RUNNING && game->status != FF_GAME_PAUSED) {
        if (callback) {
            callback(FF_ERROR_UNKNOWN, user_data);
        }
        return FF_ERROR_UNKNOWN;
    }
    
    game->status = FF_GAME_FINISHED;
    
    // 移除相关玩家
    for (size_t i = 0; i < g_player_count; ) {
        if (g_players[i].game_id == game_id) {
            memmove(&g_players[i], &g_players[i + 1], (g_player_count - i - 1) * sizeof(GamePlayer));
            g_player_count--;
        } else {
            i++;
        }
    }
    
    FF_LOG_INFO("Game ended: %llu", game_id);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_game_update_score(uint64_t game_id, uint32_t player_uid, uint32_t score, FFAsyncCallback callback, void* user_data) {
    for (size_t i = 0; i < g_player_count; i++) {
        if (g_players[i].game_id == game_id && g_players[i].uid == player_uid) {
            g_players[i].score = score;
            FF_LOG_INFO("Score updated: player %u in game %llu: %u", player_uid, game_id, score);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_game_get_leaderboard(uint64_t game_id, FFGamePlayer** leaderboard, size_t* count, FFAsyncCallback callback, void* user_data) {
    return ff_game_get_players(game_id, leaderboard, count, callback, user_data);
}

void ff_game_free(FFGame* game) {
    if (game) {
        free(game);
    }
}

void ff_game_list_free(FFGame* games, size_t count) {
    if (games) {
        free(games);
    }
}

void ff_game_player_list_free(FFGamePlayer* players, size_t count) {
    if (players) {
        free(players);
    }
}

void ff_game_action_list_free(FFGameAction* actions, size_t count) {
    if (actions) {
        free(actions);
    }
}

bool ff_game_exists(uint64_t game_id) {
    for (size_t i = 0; i < g_game_count; i++) {
        if (g_games[i].game_id == game_id) {
            return true;
        }
    }
    return false;
}

bool ff_game_is_player(uint64_t game_id, uint32_t uid) {
    for (size_t i = 0; i < g_player_count; i++) {
        if (g_players[i].game_id == game_id && g_players[i].uid == uid) {
            return true;
        }
    }
    return false;
}
