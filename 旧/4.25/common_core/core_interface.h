#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 回调函数类型定义（避免与 location_manager.h 中的类型冲突）
typedef void (*CoreCallback)(void* user_data, int status, const char* message);
typedef void (*UserCallback)(void* user_data, int status, const char* user_json);
typedef void (*MessageCallback)(void* user_data, int status, const char* message_json);
typedef void (*CoreLocationCallback)(void* user_data, int status, const char* location_json);
typedef void (*GameCallback)(void* user_data, int status, const char* game_json);
typedef void (*MediaCallback)(void* user_data, int status, const char* media_json);

// 统一的核心接口初始化
bool core_interface_init(const char* server_host, int port);

// 统一的核心接口销毁
void core_interface_destroy(void);

// 获取当前状态
const char* core_interface_get_version(void);
bool core_interface_is_initialized(void);

// ============ 用户管理接口 ============

// 用户登录
void core_login(const char* username, const char* password, UserCallback callback, void* user_data);

// 用户注册
void core_register(const char* username, const char* password, const char* nickname, UserCallback callback, void* user_data);

// 用户登出
void core_logout(void);

// 获取当前用户信息
void core_get_current_user(UserCallback callback, void* user_data);

// 根据 ID 获取用户信息
void core_get_user_by_id(uint32_t user_id, UserCallback callback, void* user_data);

// 更新用户信息
void core_update_user(const char* user_json, UserCallback callback, void* user_data);

// ============ 消息管理接口 ============

// 发送消息
void core_send_message(uint32_t to_id, const char* message, MessageCallback callback, void* user_data);

// 获取消息列表
void core_get_messages(int count, MessageCallback callback, void* user_data);

// 标记消息已读
void core_mark_message_read(uint32_t message_id, MessageCallback callback, void* user_data);

// 删除消息
void core_delete_message(uint32_t message_id, MessageCallback callback, void* user_data);

// ============ 位置管理接口 ============

// 上报位置
void core_report_location(double latitude, double longitude, CoreLocationCallback callback, void* user_data);

// 获取附近用户
void core_get_nearby_users(double latitude, double longitude, double radius_meters, CoreLocationCallback callback, void* user_data);

// 获取用户位置
void core_get_user_location(uint32_t user_id, CoreLocationCallback callback, void* user_data);

// 开启/关闭位置共享
void core_set_location_sharing(bool enabled, CoreLocationCallback callback, void* user_data);

// ============ 游戏逻辑接口 ============

// 获取游戏列表
void core_get_games(GameCallback callback, void* user_data);

// 加入游戏
void core_join_game(uint32_t game_id, GameCallback callback, void* user_data);

// 离开游戏
void core_leave_game(uint32_t game_id, GameCallback callback, void* user_data);

// 游戏动作
void core_game_action(uint32_t game_id, const char* action_json, GameCallback callback, void* user_data);

// ============ 多媒体传输接口 ============

// 上传媒体文件
void core_upload_media(const char* file_path, const char* media_type, MediaCallback callback, void* user_data);

// 下载媒体文件
void core_download_media(const char* media_id, const char* save_path, MediaCallback callback, void* user_data);

// 删除媒体文件
void core_delete_media(const char* media_id, MediaCallback callback, void* user_data);

// 获取媒体信息
void core_get_media_info(const char* media_id, MediaCallback callback, void* user_data);

// ============ 调试接口 ============

// 执行调试命令
void core_debug_command(const char* command, CoreCallback callback, void* user_data);

// 获取调试日志
void core_debug_get_logs(int last_lines, CoreCallback callback, void* user_data);

// 清理调试日志
void core_debug_clear_logs(void);

// 获取模块状态
const char* core_debug_get_module_status(void);

// ============ 平台特定接口 ============

#if defined(__ANDROID__)
void core_set_android_context(void* context);
#endif

#if defined(_WIN32) || defined(_WIN64)
void core_set_hwnd(void* hwnd);
void core_set_instance(void* instance);
#endif

#ifdef __cplusplus
}
#endif

#endif // CORE_INTERFACE_H
