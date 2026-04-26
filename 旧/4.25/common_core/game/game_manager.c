#include "game_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 本地游戏数据存储路径
#define GAME_DATA_DIR "./game_data"
#define GAME_LIST_FILE "./game_data/game_list.dat"
#define GAME_DATA_FILE "./game_data/game_data.dat"

// 简单的本地加密函数
static void encrypt_data(char* data, size_t length) {
    // 简单的异或加密，适合本地存储
    char key = 0x55;
    for (size_t i = 0; i < length; i++) {
        data[i] ^= key;
    }
}

// 简单的本地解密函数
static void decrypt_data(char* data, size_t length) {
    // 与加密相同的异或操作
    char key = 0x55;
    for (size_t i = 0; i < length; i++) {
        data[i] ^= key;
    }
}

// 确保游戏数据目录存在
static bool ensure_game_data_dir(void) {
    // 尝试创建目录
    char command[256];
    snprintf(command, 256, "mkdir -p %s", GAME_DATA_DIR);
    system(command);
    return true;
}

// 初始化游戏管理器
GameManager* game_manager_init(void) {
    GameManager* manager = (GameManager*)malloc(sizeof(GameManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory for game manager\n");
        return NULL;
    }
    
    // 初始化结构体
    memset(manager, 0, sizeof(GameManager));
    
    // 确保游戏数据目录存在
    ensure_game_data_dir();
    
    // 初始化游戏列表
    manager->game_count = 3;
    manager->games = (Game*)malloc(sizeof(Game) * manager->game_count);
    if (!manager->games) {
        fprintf(stderr, "Error allocating memory for games\n");
        free(manager);
        return NULL;
    }
    
    // 初始化默认游戏数据
    strcpy(manager->games[0].game_id, "game_001");
    strcpy(manager->games[0].name, "贪吃蛇");
    strcpy(manager->games[0].description, "经典的贪吃蛇游戏，控制蛇吃食物并成长");
    strcpy(manager->games[0].icon_url, "snake_icon.png");
    strcpy(manager->games[0].version, "1.0.0");
    manager->games[0].status = 1;
    manager->games[0].required_permission = 0;
    
    strcpy(manager->games[1].game_id, "game_002");
    strcpy(manager->games[1].name, "俄罗斯方块");
    strcpy(manager->games[1].description, "经典的俄罗斯方块游戏，堆叠方块消除行");
    strcpy(manager->games[1].icon_url, "tetris_icon.png");
    strcpy(manager->games[1].version, "1.0.0");
    manager->games[1].status = 1;
    manager->games[1].required_permission = 0;
    
    strcpy(manager->games[2].game_id, "game_003");
    strcpy(manager->games[2].name, "五子棋");
    strcpy(manager->games[2].description, "经典的五子棋游戏，先连成五子者胜");
    strcpy(manager->games[2].icon_url, "gobang_icon.png");
    strcpy(manager->games[2].version, "1.0.0");
    manager->games[2].status = 1;
    manager->games[2].required_permission = 0;
    
    // 初始化游戏数据列表
    manager->game_data_count = 0;
    manager->game_data = NULL;
    
    manager->initialized = true;
    return manager;
}

// 销毁游戏管理器
void game_manager_destroy(GameManager* manager) {
    if (manager) {
        if (manager->games) {
            free(manager->games);
        }
        if (manager->game_data) {
            free(manager->game_data);
        }
        free(manager);
    }
}

// 获取游戏列表
bool game_manager_get_games(GameManager* manager, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized) {
        return false;
    }
    
    // 模拟游戏列表数据
    // 实际应用中应该从本地文件读取
    if (callback) {
        // 简单的JSON格式模拟数据
        char response[1024];
        snprintf(response, 1024, "{\"games\":[\
" 
                 "{\"game_id\":\"game_001\",\"name\":\"贪吃蛇\",\"description\":\"经典的贪吃蛇游戏\",\"status\":1},\
" 
                 "{\"game_id\":\"game_002\",\"name\":\"俄罗斯方块\",\"description\":\"经典的俄罗斯方块游戏\",\"status\":1},\
" 
                 "{\"game_id\":\"game_003\",\"name\":\"五子棋\",\"description\":\"经典的五子棋游戏\",\"status\":1}\
" 
                 "]}");
        
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 获取游戏详情
bool game_manager_get_game_detail(GameManager* manager, const char* game_id, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id) {
        return false;
    }
    
    // 查找游戏
    Game* game = NULL;
    for (int i = 0; i < manager->game_count; i++) {
        if (strcmp(manager->games[i].game_id, game_id) == 0) {
            game = &manager->games[i];
            break;
        }
    }
    
    if (game && callback) {
        // 简单的JSON格式模拟数据
        char response[1024];
        snprintf(response, 1024, "{\"game_id\":\"%s\",\"name\":\"%s\",\"description\":\"%s\",\"icon_url\":\"%s\",\"version\":\"%s\",\"status\":%d,\"required_permission\":%d}",
                 game->game_id, game->name, game->description, game->icon_url, game->version, game->status, game->required_permission);
        
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 检查游戏权限
bool game_manager_check_permission(GameManager* manager, const char* game_id, int permission, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id) {
        return false;
    }
    
    // 本地权限检查，始终返回成功
    if (callback) {
        char response[256];
        snprintf(response, 256, "{\"game_id\":\"%s\",\"permission\":%d,\"granted\":true}", game_id, permission);
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 同步游戏数据
bool game_manager_sync_game_data(GameManager* manager, const char* game_id, const char* data, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id || !data) {
        return false;
    }
    
    // 本地存储游戏数据
    char file_path[256];
    snprintf(file_path, 256, "%s/%s.dat", GAME_DATA_DIR, game_id);
    
    FILE* file = fopen(file_path, "w");
    if (file) {
        // 加密数据后存储
        char encrypted_data[1024];
        strncpy(encrypted_data, data, 1023);
        encrypted_data[1023] = '\0';
        encrypt_data(encrypted_data, strlen(encrypted_data));
        
        fprintf(file, "%s", encrypted_data);
        fclose(file);
    }
    
    // 更新内存中的游戏数据
    bool found = false;
    for (int i = 0; i < manager->game_data_count; i++) {
        if (strcmp(manager->game_data[i].game_id, game_id) == 0) {
            strcpy(manager->game_data[i].data, data);
            manager->game_data[i].last_sync = time(NULL);
            manager->game_data[i].sync_status = 1;
            found = true;
            break;
        }
    }
    
    if (!found) {
        // 添加新的游戏数据
        manager->game_data_count++;
        manager->game_data = (GameData*)realloc(manager->game_data, sizeof(GameData) * manager->game_data_count);
        if (manager->game_data) {
            GameData* new_data = &manager->game_data[manager->game_data_count - 1];
            strcpy(new_data->game_id, game_id);
            strcpy(new_data->user_id, "local_user");
            strcpy(new_data->data, data);
            new_data->last_sync = time(NULL);
            new_data->sync_status = 1;
        }
    }
    
    if (callback) {
        char response[256];
        snprintf(response, 256, "{\"game_id\":\"%s\",\"sync_status\":1,\"last_sync\":%ld}", game_id, time(NULL));
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 获取游戏数据
bool game_manager_get_game_data(GameManager* manager, const char* game_id, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id) {
        return false;
    }
    
    // 从本地文件读取游戏数据
    char file_path[256];
    snprintf(file_path, 256, "%s/%s.dat", GAME_DATA_DIR, game_id);
    
    char data[1024] = "{}";
    FILE* file = fopen(file_path, "r");
    if (file) {
        fgets(data, 1024, file);
        fclose(file);
        
        // 解密数据
        decrypt_data(data, strlen(data));
    }
    
    // 检查内存中的游戏数据
    for (int i = 0; i < manager->game_data_count; i++) {
        if (strcmp(manager->game_data[i].game_id, game_id) == 0) {
            strcpy(data, manager->game_data[i].data);
            break;
        }
    }
    
    if (callback) {
        char response[1024];
        snprintf(response, 1024, "{\"game_id\":\"%s\",\"data\":%s,\"last_sync\":%ld}", game_id, data, time(NULL));
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 绑定游戏账号
bool game_manager_bind_game_account(GameManager* manager, const char* game_id, const char* account_id, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id || !account_id) {
        return false;
    }
    
    // 本地存储账号绑定信息
    char file_path[256];
    snprintf(file_path, 256, "%s/%s_account.dat", GAME_DATA_DIR, game_id);
    
    FILE* file = fopen(file_path, "w");
    if (file) {
        // 加密存储
        char encrypted_account[256];
        strncpy(encrypted_account, account_id, 255);
        encrypted_account[255] = '\0';
        encrypt_data(encrypted_account, strlen(encrypted_account));
        
        fprintf(file, "%s", encrypted_account);
        fclose(file);
    }
    
    if (callback) {
        char response[256];
        snprintf(response, 256, "{\"game_id\":\"%s\",\"account_id\":\"%s\",\"bound\":true}", game_id, account_id);
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 推送游戏通知
bool game_manager_push_game_notification(GameManager* manager, const char* game_id, const char* message, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id || !message) {
        return false;
    }
    
    // 本地模拟通知推送
    printf("Game notification: %s - %s\n", game_id, message);
    
    if (callback) {
        char response[256];
        snprintf(response, 256, "{\"game_id\":\"%s\",\"message\":\"%s\",\"pushed\":true}", game_id, message);
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}

// 扩展游戏功能
bool game_manager_extend_game_function(GameManager* manager, const char* game_id, const char* function_name, const char* parameters, GameCallback callback, void* user_data) {
    if (!manager || !manager->initialized || !game_id || !function_name) {
        return false;
    }
    
    // 本地模拟扩展功能
    printf("Extend game function: %s - %s(%s)\n", game_id, function_name, parameters ? parameters : "");
    
    if (callback) {
        char response[256];
        snprintf(response, 256, "{\"game_id\":\"%s\",\"function\":\"%s\",\"executed\":true}", game_id, function_name);
        callback(user_data, 200, (const uint8_t*)response, strlen(response));
    }
    
    return true;
}