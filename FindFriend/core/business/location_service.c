// FindFriend 位置服务模块

#include "ff_core_interface.h"
#include "ff_location_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_LOCATIONS 1024
#define EARTH_RADIUS 6371.0 // 地球半径(km)

// 位置数据结构
typedef struct {
    uint32_t uid;
    double latitude;
    double longitude;
    double accuracy;
    uint64_t timestamp;
    uint8_t source;
    FFPrivacyLevel privacy_level;
} Location;

static Location g_locations[MAX_LOCATIONS];
static size_t g_location_count = 0;
static FFPrivacyLevel g_global_privacy_level = FF_PRIVACY_EXACT;

FFResult ff_location_init(void) {
    g_location_count = 0;
    memset(g_locations, 0, sizeof(g_locations));
    g_global_privacy_level = FF_PRIVACY_EXACT;
    FF_LOG_INFO("Location service initialized");
    return FF_OK;
}

void ff_location_cleanup(void) {
    g_location_count = 0;
    FF_LOG_INFO("Location service cleanup");
}

float calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    double dlat = (lat2 - lat1) * M_PI / 180.0;
    double dlon = (lon2 - lon1) * M_PI / 180.0;
    double a = sin(dlat/2) * sin(dlat/2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dlon/2) * sin(dlon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return (float)(EARTH_RADIUS * c * 1000.0); // 转换为米
}

FFResult ff_location_update(double latitude, double longitude, double accuracy, FFAsyncCallback callback, void* user_data) {
    // 验证坐标范围
    if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，使用固定UID 1000
    uint32_t uid = 1000;
    
    // 查找现有位置记录
    for (size_t i = 0; i < g_location_count; i++) {
        if (g_locations[i].uid == uid) {
            g_locations[i].latitude = latitude;
            g_locations[i].longitude = longitude;
            g_locations[i].accuracy = accuracy;
            g_locations[i].timestamp = time(NULL);
            g_locations[i].source = 0; // GPS
            FF_LOG_INFO("Location updated: %.6f, %.6f", latitude, longitude);
            
            if (callback) {
                callback(FF_OK, user_data);
            }
            
            return FF_OK;
        }
    }
    
    // 创建新位置记录
    if (g_location_count >= MAX_LOCATIONS) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    Location* loc = &g_locations[g_location_count];
    loc->uid = uid;
    loc->latitude = latitude;
    loc->longitude = longitude;
    loc->accuracy = accuracy;
    loc->timestamp = time(NULL);
    loc->source = 0; // GPS
    loc->privacy_level = g_global_privacy_level;
    
    g_location_count++;
    FF_LOG_INFO("Location set: %.6f, %.6f", latitude, longitude);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_get_current(FFLocation** location, FFAsyncCallback callback, void* user_data) {
    if (!location) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，返回第一个位置
    if (g_location_count > 0) {
        *location = (FFLocation*)malloc(sizeof(FFLocation));
        if (!*location) {
            if (callback) {
                callback(FF_ERROR_NO_MEMORY, user_data);
            }
            return FF_ERROR_NO_MEMORY;
        }
        
        Location* loc = &g_locations[0];
        (*location)->latitude = loc->latitude;
        (*location)->longitude = loc->longitude;
        (*location)->accuracy = loc->accuracy;
        (*location)->timestamp = loc->timestamp;
        (*location)->source = loc->source;
        
        if (callback) {
            callback(FF_OK, user_data);
        }
        
        return FF_OK;
    }
    
    if (callback) {
        callback(FF_ERROR_NOT_FOUND, user_data);
    }
    
    return FF_ERROR_NOT_FOUND;
}

FFResult ff_location_set_privacy_level(FFPrivacyLevel level) {
    if (level > FF_PRIVACY_HIDDEN) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    g_global_privacy_level = level;
    
    // 更新所有现有位置的隐私级别
    for (size_t i = 0; i < g_location_count; i++) {
        g_locations[i].privacy_level = level;
    }
    
    FF_LOG_INFO("Privacy level set to %u", level);
    return FF_OK;
}

FFResult ff_location_get_privacy_level(FFPrivacyLevel* level) {
    if (!level) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *level = g_global_privacy_level;
    return FF_OK;
}

FFResult ff_location_share_with_friend(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    FF_LOG_INFO("Sharing location with friend: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_stop_share_with_friend(uint32_t uid, FFAsyncCallback callback, void* user_data) {
    FF_LOG_INFO("Stopped sharing location with friend: UID %u", uid);
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_share_with_all_friends(FFAsyncCallback callback, void* user_data) {
    FF_LOG_INFO("Sharing location with all friends");
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_stop_share_with_all(FFAsyncCallback callback, void* user_data) {
    FF_LOG_INFO("Stopped sharing location with all friends");
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_can_view_exact(uint32_t viewer_uid, bool* can_view) {
    if (!can_view) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，始终返回 true
    *can_view = true;
    return FF_OK;
}

FFResult ff_location_get_friend_sharing_list(uint32_t** uids, size_t* count) {
    if (!uids || !count) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，返回空列表
    *uids = NULL;
    *count = 0;
    return FF_OK;
}

FFResult ff_location_get_nearby_users(double latitude, double longitude, double radius_meters, FFNearbyUser** users, size_t* count, FFAsyncCallback callback, void* user_data) {
    if (!users || !count) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    size_t nearby_count = 0;
    for (size_t i = 0; i < g_location_count; i++) {
        float distance = calculate_distance(latitude, longitude, g_locations[i].latitude, g_locations[i].longitude);
        if (distance <= radius_meters) {
            nearby_count++;
        }
    }
    
    if (nearby_count == 0) {
        *users = NULL;
        *count = 0;
        if (callback) {
            callback(FF_OK, user_data);
        }
        return FF_OK;
    }
    
    *users = (FFNearbyUser*)malloc(nearby_count * sizeof(FFNearbyUser));
    if (!*users) {
        if (callback) {
            callback(FF_ERROR_NO_MEMORY, user_data);
        }
        return FF_ERROR_NO_MEMORY;
    }
    
    size_t index = 0;
    for (size_t i = 0; i < g_location_count; i++) {
        float distance = calculate_distance(latitude, longitude, g_locations[i].latitude, g_locations[i].longitude);
        if (distance <= radius_meters) {
            Location* loc = &g_locations[i];
            (*users)[index].uid = loc->uid;
            strncpy((*users)[index].username, "user", sizeof((*users)[index].username) - 1);
            (*users)[index].latitude = loc->latitude;
            (*users)[index].longitude = loc->longitude;
            (*users)[index].distance = distance;
            (*users)[index].last_active = loc->timestamp;
            (*users)[index].is_friend = false;
            (*users)[index].sharing_enabled = true;
            index++;
        }
    }
    
    *count = nearby_count;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_get_nearby_friends(double latitude, double longitude, double radius_meters, FFNearbyUser** friends, size_t* count, FFAsyncCallback callback, void* user_data) {
    // 简化实现，返回空列表
    *friends = NULL;
    *count = 0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_get_distance_to_friend(uint32_t uid, double* distance_meters, FFAsyncCallback callback, void* user_data) {
    if (!distance_meters) {
        if (callback) {
            callback(FF_ERROR_INVALID_PARAM, user_data);
        }
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，返回固定距离
    *distance_meters = 100.0;
    
    if (callback) {
        callback(FF_OK, user_data);
    }
    
    return FF_OK;
}

FFResult ff_location_calculate_distance(double lat1, double lon1, double lat2, double lon2, double* distance_meters) {
    if (!distance_meters) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *distance_meters = calculate_distance(lat1, lon1, lat2, lon2);
    return FF_OK;
}

FFResult ff_location_obfuscate(double exact_lat, double exact_lon, double radius_meters, double* obfuscated_lat, double* obfuscated_lon) {
    if (!obfuscated_lat || !obfuscated_lon) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，返回原始坐标
    *obfuscated_lat = exact_lat;
    *obfuscated_lon = exact_lon;
    return FF_OK;
}

FFResult ff_location_get_friend_distance_fuzzy(double exact_distance_meters, double* fuzzy_distance_meters) {
    if (!fuzzy_distance_meters) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化实现，返回原始距离
    *fuzzy_distance_meters = exact_distance_meters;
    return FF_OK;
}

void ff_location_free(FFLocation* location) {
    if (location) {
        free(location);
    }
}

void ff_nearby_user_list_free(FFNearbyUser* users, size_t count) {
    if (users) {
        free(users);
    }
}

bool ff_location_exists(uint32_t uid) {
    for (size_t i = 0; i < g_location_count; i++) {
        if (g_locations[i].uid == uid) {
            return true;
        }
    }
    return false;
}
