#ifndef GOOD_SAMARITAN_H
#define GOOD_SAMARITAN_H

#include <stdint.h>
#include <netinet/in.h>

typedef struct {
    uint32_t user_id;
    char nickname[64];
    uint8_t avatar_hash[32];
    double latitude;
    double longitude;
    char city[64];
    uint32_t ip_addr;
    uint16_t port;
    uint32_t last_seen;
    uint16_t tags;
    char signature[128];
} UserLocation;

typedef struct {
    uint32_t count;
    UserLocation users[50];
} NearbyResponse;

int start_good_samaritan(int port);
void stop_good_samaritan(void);
int report_location(UserLocation *loc);
int get_nearby_users(double lat, double lon, double radius, NearbyResponse *resp);

#endif
