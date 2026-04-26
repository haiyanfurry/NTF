// FindFriend P2P 位置共享实现

#include "ff_p2p_location.h"
#include "ff_core_interface.h"
#include "ff_p2p_friend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOCATION_UPDATE_INTERVAL 60
#define MIN_DISTANCE_FOR_UPDATE 100.0
#define APPROXIMATE_RADIUS_KM 1.0

static P2PLocation g_my_location;
static bool g_location_enabled = false;
static uint64_t g_last_update_time = 0;

FFResult ff_location_init(void) {
    g_my_location.latitude = 0.0;
    g_my_location.longitude = 0.0;
    g_my_location.timestamp = 0;
    g_my_location.is_approximate = false;
    g_location_enabled = false;
    g_last_update_time = 0;
    return FF_OK;
}

void ff_location_cleanup(void) {
    g_location_enabled = false;
}

FFResult ff_location_update(double latitude, double longitude, bool is_approximate) {
    if (latitude < -90.0 || latitude > 90.0 ||
        longitude < -180.0 || longitude > 180.0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    g_my_location.latitude = latitude;
    g_my_location.longitude = longitude;
    g_my_location.timestamp = time(NULL);
    g_my_location.is_approximate = is_approximate;
    g_location_enabled = true;
    g_last_update_time = g_my_location.timestamp;
    
    return FF_OK;
}

FFResult ff_location_get_mine(P2PLocation* location) {
    if (!location) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_location_enabled) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    *location = g_my_location;
    return FF_OK;
}

FFResult ff_location_share_with_friend(const uint8_t* friend_uid, FFAsyncCallback callback, void* user_data) {
    if (!friend_uid) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    bool is_friend;
    FFResult result = ff_friend_is_friend(friend_uid, &is_friend);
    if (result != FF_OK || !is_friend) {
        return FF_ERROR;
    }
    
    P2PLocation share_location = g_my_location;
    share_location.is_approximate = true;
    
    result = ff_friend_update_location(friend_uid, &share_location);
    
    if (callback) {
        callback(result, user_data);
    }
    
    return result;
}

FFResult ff_location_get_friend_location(const uint8_t* friend_uid, P2PLocation* location, bool* is_approximate) {
    if (!friend_uid || !location) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    bool shared;
    FFResult result = ff_friend_get_location(friend_uid, location, &shared);
    if (result != FF_OK) {
        return result;
    }
    
    if (is_approximate) {
        *is_approximate = location->is_approximate;
    }
    
    return FF_OK;
}

double ff_location_calculate_distance(const P2PLocation* loc1, const P2PLocation* loc2) {
    if (!loc1 || !loc2) {
        return -1.0;
    }
    
    const double R = 6371.0;
    
    double lat1_rad = loc1->latitude * M_PI / 180.0;
    double lat2_rad = loc2->latitude * M_PI / 180.0;
    double delta_lat = (loc2->latitude - loc1->latitude) * M_PI / 180.0;
    double delta_lon = (loc2->longitude - loc1->longitude) * M_PI / 180.0;
    
    double a = sin(delta_lat / 2.0) * sin(delta_lat / 2.0) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2.0) * sin(delta_lon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    
    return R * c * 1000.0;
}

FFResult ff_location_calculate_distance_between_friends(const uint8_t* friend1_uid,
                                                       const uint8_t* friend2_uid,
                                                       double* distance) {
    if (!friend1_uid || !friend2_uid || !distance) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    P2PLocation loc1, loc2;
    
    FFResult result = ff_friend_get_location(friend1_uid, &loc1, NULL);
    if (result != FF_OK) {
        return result;
    }
    
    result = ff_friend_get_location(friend2_uid, &loc2, NULL);
    if (result != FF_OK) {
        return result;
    }
    
    *distance = ff_location_calculate_distance(&loc1, &loc2);
    
    return FF_OK;
}

char* ff_location_format_distance(double distance_meters) {
    static char buffer[128];
    
    if (distance_meters < 0) {
        snprintf(buffer, sizeof(buffer), "Unknown");
    } else if (distance_meters < 1000) {
        snprintf(buffer, sizeof(buffer), "%.0f m", distance_meters);
    } else if (distance_meters < 10000) {
        snprintf(buffer, sizeof(buffer), "%.1f km", distance_meters / 1000.0);
    } else {
        snprintf(buffer, sizeof(buffer), "%.0f km", distance_meters / 1000.0);
    }
    
    return buffer;
}

char* ff_location_format_approximate_distance(double distance_meters) {
    static char buffer[128];
    
    if (distance_meters < 0) {
        snprintf(buffer, sizeof(buffer), "Distance unknown");
    } else if (distance_meters < 500) {
        snprintf(buffer, sizeof(buffer), "Very close (< 500m)");
    } else if (distance_meters < 2000) {
        snprintf(buffer, sizeof(buffer), "Nearby (~1 km)");
    } else if (distance_meters < 5000) {
        snprintf(buffer, sizeof(buffer), "Within a few km");
    } else if (distance_meters < 10000) {
        snprintf(buffer, sizeof(buffer), "About 10 km away");
    } else {
        snprintf(buffer, sizeof(buffer), "Far away (> 10 km)");
    }
    
    return buffer;
}

FFResult ff_location_should_update(double new_lat, double new_lon) {
    if (!g_location_enabled) {
        return FF_OK;
    }
    
    uint64_t now = time(NULL);
    if (now - g_last_update_time < LOCATION_UPDATE_INTERVAL) {
        P2PLocation temp_loc = {new_lat, new_lon, now, false};
        double distance = ff_location_calculate_distance(&g_my_location, &temp_loc);
        if (distance < MIN_DISTANCE_FOR_UPDATE) {
            return FF_ERROR;
        }
    }
    
    return FF_OK;
}

P2PLocation ff_location_blur(const P2PLocation* original, double radius_km) {
    P2PLocation blurred = *original;
    
    if (!original || radius_km <= 0) {
        return blurred;
    }
    
    double radius_deg = radius_km / 111.0;
    
    double random_offset_lat = ((double)rand() / RAND_MAX - 0.5) * 2.0 * radius_deg;
    double random_offset_lon = ((double)rand() / RAND_MAX - 0.5) * 2.0 * radius_deg / cos(original->latitude * M_PI / 180.0);
    
    blurred.latitude = original->latitude + random_offset_lat;
    blurred.longitude = original->longitude + random_offset_lon;
    blurred.is_approximate = true;
    
    if (blurred.latitude < -90.0) blurred.latitude = -90.0;
    if (blurred.latitude > 90.0) blurred.latitude = 90.0;
    if (blurred.longitude < -180.0) blurred.longitude = -180.0;
    if (blurred.longitude > 180.0) blurred.longitude = 180.0;
    
    return blurred;
}

FFResult ff_location_share_blurred(const uint8_t* friend_uid, double radius_km, FFAsyncCallback callback, void* user_data) {
    if (!friend_uid) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (!g_location_enabled) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    P2PLocation blurred = ff_location_blur(&g_my_location, radius_km);
    
    bool is_friend;
    FFResult result = ff_friend_is_friend(friend_uid, &is_friend);
    if (result != FF_OK || !is_friend) {
        return FF_ERROR;
    }
    
    result = ff_friend_update_location(friend_uid, &blurred);
    
    if (callback) {
        callback(result, user_data);
    }
    
    return result;
}

bool ff_location_is_enabled(void) {
    return g_location_enabled;
}

void ff_location_disable(void) {
    g_location_enabled = false;
}
