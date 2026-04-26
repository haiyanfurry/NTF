#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <time.h>
#include <pthread.h>

// 网络错误码
typedef enum {
    NETWORK_SUCCESS = 0,
    NETWORK_ERROR = 1,
    NETWORK_TIMEOUT = 2,
    NETWORK_CONNECTION_FAILED = 3,
    NETWORK_COOLDOWN = 4
} NetworkError;

// 设备信息结构体
typedef struct {
    char ip[16];
    char name[64];
    char type[32]; // "organizer" 或 "user"
    time_t last_seen;
} DeviceInfo;

// 传输任务结构体
typedef struct {
    uint32_t id;
    uint32_t from_id;
    uint32_t to_id;
    char file_path[512];
    char original_name[256];
    int file_size;
    int type;           // 1:图片 2:音频 3:视频
    int progress;
    int status;         // 0:等待 1:传输中 2:完成 3:失败
    pthread_t thread;
} TransferTask;

// 批量传输任务结构体
typedef struct {
    uint32_t count;
    TransferTask tasks[250];
    int total_count;
    int completed_count;
} BatchTransfer;

// 初始化网络模块
int network_init(void);

// 关闭网络模块
void network_close(void);

// 老好人服务器
int network_start_good_samaritan(int port);
void network_stop_good_samaritan(void);

// 位置和用户
int network_report_location(void *loc);
int network_get_nearby_users(double lat, double lon, double radius, void *list);
int network_send_friend_request(uint32_t target_id);
int network_check_cooldown(uint32_t target_id);

// IP快速切换
int network_set_server_ip(const char *ip);
const char* network_get_current_server_ip(void);
int network_add_server_ip(const char *ip, const char *name);
int network_remove_server_ip(int index);
int network_get_server_ip_list(char ***ips, char ***names, int *count);

// 同城用户发现
int network_start_local_discovery(void);
void network_stop_local_discovery(void);
int network_get_device_list(DeviceInfo **device_list, int *count);
int network_connect_to_device(const char *ip, int port);

// 媒体传输
int network_media_transfer_init(void);
int network_media_transfer_send_batch(uint32_t to_id, char **file_paths, int count, int type);
typedef void (*MediaReceiveCallback)(uint32_t from_id, char *file_path, char *original_name, int type);
void network_media_transfer_set_callback(MediaReceiveCallback cb);
int network_media_transfer_get_progress(uint32_t task_id);
int network_media_transfer_cancel(uint32_t task_id);

#endif