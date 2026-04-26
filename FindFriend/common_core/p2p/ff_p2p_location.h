// FindFriend P2P 位置共享接口

#ifndef FF_P2P_LOCATION_H
#define FF_P2P_LOCATION_H

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#define LOCATION_UPDATE_INTERVAL 60
#define MIN_DISTANCE_FOR_UPDATE 100.0
#define APPROXIMATE_RADIUS_KM 1.0

FFResult ff_location_init(void);

void ff_location_cleanup(void);

FFResult ff_location_update(double latitude, double longitude, bool is_approximate);

FFResult ff_location_get_mine(P2PLocation* location);

FFResult ff_location_share_with_friend(const uint8_t* friend_uid, FFAsyncCallback callback, void* user_data);

FFResult ff_location_get_friend_location(const uint8_t* friend_uid, P2PLocation* location, bool* is_approximate);

double ff_location_calculate_distance(const P2PLocation* loc1, const P2PLocation* loc2);

FFResult ff_location_calculate_distance_between_friends(const uint8_t* friend1_uid,
                                                       const uint8_t* friend2_uid,
                                                       double* distance);

char* ff_location_format_distance(double distance_meters);

char* ff_location_format_approximate_distance(double distance_meters);

FFResult ff_location_should_update(double new_lat, double new_lon);

P2PLocation ff_location_blur(const P2PLocation* original, double radius_km);

FFResult ff_location_share_blurred(const uint8_t* friend_uid, double radius_km, FFAsyncCallback callback, void* user_data);

bool ff_location_is_enabled(void);

void ff_location_disable(void);

#endif // FF_P2P_LOCATION_H
