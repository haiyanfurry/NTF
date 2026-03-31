#ifndef GOOD_SAMARITAN_H
#define GOOD_SAMARITAN_H

#include <stdint.h>
#include <time.h>

int start_good_samaritan(int port);
void stop_good_samaritan(void);
int report_location(void *loc);
int get_nearby_users(double lat, double lon, double radius, void *list);
int send_friend_request(uint32_t target_id);
int check_cooldown(uint32_t target_id);

// IP快速切换功能
int set_server_ip(const char *ip);
const char* get_current_server_ip(void);
int add_server_ip(const char *ip, const char *name);
int remove_server_ip(int index);
int get_server_ip_list(char ***ips, char ***names, int *count);

// 同城用户发现功能
typedef struct {
    char ip[16];
    char name[64];
    char type[32]; // "organizer" 或 "user"
    time_t last_seen;
} DeviceInfo;

int start_local_discovery(void);
void stop_local_discovery(void);
int get_device_list(DeviceInfo **device_list, int *count);
int connect_to_device(const char *ip, int port);

#endif
