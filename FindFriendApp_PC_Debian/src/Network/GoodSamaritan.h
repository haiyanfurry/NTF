#ifndef GOOD_SAMARITAN_H
#define GOOD_SAMARITAN_H

#include <stdint.h>

int start_good_samaritan(int port);
void stop_good_samaritan(void);
int report_location(void *loc);
int get_nearby_users(double lat, double lon, double radius, void *list);
int send_friend_request(uint32_t target_id);
int check_cooldown(uint32_t target_id);

#endif
