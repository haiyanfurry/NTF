#ifdef __APPLE__

// macOS 和 iOS 平台接口
#include "core_interface.h"
#include <Foundation/Foundation.h>

// Objective-C 接口类
@interface FindFriendCore : NSObject

// 初始化与销毁
- (BOOL)initWithHost:(NSString*)host port:(int)port;
- (void)destroy;

// 基本信息
- (NSString*)getVersion;
- (BOOL)isInitialized;

// 用户管理
- (void)login:(NSString*)username password:(NSString*)password;
- (void)logout;
- (void)getCurrentUser;
- (void)getUserById:(uint32_t)userId;
- (void)updateUser:(NSString*)userJson;

// 消息管理
- (void)sendMessage:(uint32_t)toId message:(NSString*)message;
- (void)getMessages:(int)count;
- (void)markMessageRead:(uint32_t)messageId;
- (void)deleteMessage:(uint32_t)messageId;

// 位置管理
- (void)reportLocation:(double)latitude longitude:(double)longitude;
- (void)getNearbyUsers:(double)latitude longitude:(double)longitude radius:(double)radius;
- (void)getUserLocation:(uint32_t)userId;
- (void)setLocationSharing:(BOOL)enabled;

// 游戏逻辑
- (void)getGames;
- (void)joinGame:(uint32_t)gameId;
- (void)leaveGame:(uint32_t)gameId;
- (void)gameAction:(uint32_t)gameId action:(NSString*)actionJson;

// 多媒体操作
- (void)uploadMedia:(NSString*)filePath mediaType:(NSString*)mediaType;
- (void)downloadMedia:(NSString*)mediaId savePath:(NSString*)savePath;
- (void)deleteMedia:(NSString*)mediaId;
- (void)getMediaInfo:(NSString*)mediaId;

// 调试接口
- (void)debugCommand:(NSString*)command;
- (void)debugGetLogs:(int)lastLines;
- (void)debugClearLogs;
- (NSString*)getModuleStatus;

@end

@implementation FindFriendCore

- (BOOL)initWithHost:(NSString*)host port:(int)port {
    return core_interface_init([host UTF8String], port);
}

- (void)destroy {
    core_interface_destroy();
}

- (NSString*)getVersion {
    return [NSString stringWithUTF8String:core_interface_get_version()];
}

- (BOOL)isInitialized {
    return core_interface_is_initialized();
}

- (void)login:(NSString*)username password:(NSString*)password {
    core_login([username UTF8String], [password UTF8String], NULL, NULL);
}

- (void)logout {
    core_logout();
}

- (void)getCurrentUser {
    core_get_current_user(NULL, NULL);
}

- (void)getUserById:(uint32_t)userId {
    core_get_user_by_id(userId, NULL, NULL);
}

- (void)updateUser:(NSString*)userJson {
    core_update_user([userJson UTF8String], NULL, NULL);
}

- (void)sendMessage:(uint32_t)toId message:(NSString*)message {
    core_send_message(toId, [message UTF8String], NULL, NULL);
}

- (void)getMessages:(int)count {
    core_get_messages(count, NULL, NULL);
}

- (void)markMessageRead:(uint32_t)messageId {
    core_mark_message_read(messageId, NULL, NULL);
}

- (void)deleteMessage:(uint32_t)messageId {
    core_delete_message(messageId, NULL, NULL);
}

- (void)reportLocation:(double)latitude longitude:(double)longitude {
    core_report_location(latitude, longitude, NULL, NULL);
}

- (void)getNearbyUsers:(double)latitude longitude:(double)longitude radius:(double)radius {
    core_get_nearby_users(latitude, longitude, radius, NULL, NULL);
}

- (void)getUserLocation:(uint32_t)userId {
    core_get_user_location(userId, NULL, NULL);
}

- (void)setLocationSharing:(BOOL)enabled {
    core_set_location_sharing(enabled, NULL, NULL);
}

- (void)getGames {
    core_get_games(NULL, NULL);
}

- (void)joinGame:(uint32_t)gameId {
    core_join_game(gameId, NULL, NULL);
}

- (void)leaveGame:(uint32_t)gameId {
    core_leave_game(gameId, NULL, NULL);
}

- (void)gameAction:(uint32_t)gameId action:(NSString*)actionJson {
    core_game_action(gameId, [actionJson UTF8String], NULL, NULL);
}

- (void)uploadMedia:(NSString*)filePath mediaType:(NSString*)mediaType {
    core_upload_media([filePath UTF8String], [mediaType UTF8String], NULL, NULL);
}

- (void)downloadMedia:(NSString*)mediaId savePath:(NSString*)savePath {
    core_download_media([mediaId UTF8String], [savePath UTF8String], NULL, NULL);
}

- (void)deleteMedia:(NSString*)mediaId {
    core_delete_media([mediaId UTF8String], NULL, NULL);
}

- (void)getMediaInfo:(NSString*)mediaId {
    core_get_media_info([mediaId UTF8String], NULL, NULL);
}

- (void)debugCommand:(NSString*)command {
    core_debug_command([command UTF8String], NULL, NULL);
}

- (void)debugGetLogs:(int)lastLines {
    core_debug_get_logs(lastLines, NULL, NULL);
}

- (void)debugClearLogs {
    core_debug_clear_logs();
}

- (NSString*)getModuleStatus {
    return [NSString stringWithUTF8String:core_debug_get_module_status()];
}

@end

#endif // __APPLE__
