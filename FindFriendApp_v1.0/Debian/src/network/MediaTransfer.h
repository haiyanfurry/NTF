#ifndef MEDIA_TRANSFER_H
#define MEDIA_TRANSFER_H

#include <stdint.h>
#include <pthread.h>

// 媒体文件传输
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

// 批量传输任务
typedef struct {
    uint32_t count;
    TransferTask tasks[250];
    int total_count;
    int completed_count;
} BatchTransfer;

// 初始化传输模块
int media_transfer_init(void);

// 发送媒体文件（支持批量）
int media_transfer_send_batch(uint32_t to_id, char **file_paths, int count, int type);

// 接收媒体文件回调
typedef void (*MediaReceiveCallback)(uint32_t from_id, char *file_path, char *original_name, int type);
void media_transfer_set_callback(MediaReceiveCallback cb);

// 获取传输进度
int media_transfer_get_progress(uint32_t task_id);

// 取消传输
int media_transfer_cancel(uint32_t task_id);

#endif
