// ====================================================================
// FindFriend 位置管理接口
// 业务模块：位置上报、附近用户、位置隐私
// ====================================================================

#ifndef FF_LOCATION_INTERFACE_H
#define FF_LOCATION_INTERFACE_H

#include "ff_core_interface.h"
#include <stdint.h>

// ====================================================================
// 位置数据结构
// ====================================================================
typedef struct FFLocation {
    double latitude;
    double longitude;
    double accuracy;      // 精度（米）
    uint64_t timestamp;
    uint8_t source;       // 0=GPS, 1=网络, 2=模糊
} FFLocation;

typedef struct FFNearbyUser {
    uint32_t uid;
    char username[64];
    double latitude;
    double longitude;
    double distance;      // 距离（米）
    uint64_t last_active;
    bool is_friend;       // 是否是好友
    bool sharing_enabled;  // 是否开启位置共享
} FFNearbyUser;

// ====================================================================
// 位置隐私级别
// ====================================================================
typedef enum FFPrivacyLevel {
    FF_PRIVACY_EXACT = 0,      // 精确位置（仅好友可见）
    FF_PRIVACY_FRIEND_FAR = 1, // 好友可见，模糊1km
    FF_PRIVACY_NEARBY = 2,     // 附近可见，模糊5km
    FF_PRIVACY_HIDDEN = 3      // 完全隐藏
} FFPrivacyLevel;

// ====================================================================
// 位置管理接口
// ====================================================================

// 位置上报
FFResult ff_location_update(double latitude, double longitude, double accuracy, FFAsyncCallback callback, void* user_data);
FFResult ff_location_get_current(FFLocation** location, FFAsyncCallback callback, void* user_data);

// 位置隐私设置
FFResult ff_location_set_privacy_level(FFPrivacyLevel level);
FFResult ff_location_get_privacy_level(FFPrivacyLevel* level);

// 位置共享
FFResult ff_location_share_with_friend(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_location_stop_share_with_friend(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_location_share_with_all_friends(FFAsyncCallback callback, void* user_data);
FFResult ff_location_stop_share_with_all(FFAsyncCallback callback, void* user_data);

// 位置共享权限查询
FFResult ff_location_can_view_exact(uint32_t viewer_uid, bool* can_view);
FFResult ff_location_get_friend_sharing_list(uint32_t** uids, size_t* count);

// 附近用户查询
FFResult ff_location_get_nearby_users(double latitude, double longitude, double radius_meters, FFNearbyUser** users, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_location_get_nearby_friends(double latitude, double longitude, double radius_meters, FFNearbyUser** friends, size_t* count, FFAsyncCallback callback, void* user_data);

// 距离计算
FFResult ff_location_get_distance_to_friend(uint32_t uid, double* distance_meters, FFAsyncCallback callback, void* user_data);
FFResult ff_location_calculate_distance(double lat1, double lon1, double lat2, double lon2, double* distance_meters);

// 位置模糊化（Rust 安全模块调用）
FFResult ff_location_obfuscate(double exact_lat, double exact_lon, double radius_meters, double* obfuscated_lat, double* obfuscated_lon);
FFResult ff_location_get_friend_distance_fuzzy(double exact_distance_meters, double* fuzzy_distance_meters);

// 内存释放
void ff_location_free(FFLocation* location);
void ff_nearby_user_list_free(FFNearbyUser* users, size_t count);

#endif // FF_LOCATION_INTERFACE_H
