#include "MediaTransfer.h"
#include <stdio.h>

int media_transfer_init(void) {
    printf("媒体传输模块初始化（占位）\n");
    return 0;
}

int media_transfer_send_batch(uint32_t to_id, char **file_paths, int count, int type) {
    printf("发送批量媒体文件（占位）\n");
    return 0;
}

void media_transfer_set_callback(MediaReceiveCallback cb) {
    (void)cb;
}

int media_transfer_get_progress(uint32_t task_id) {
    return 0;
}

int media_transfer_cancel(uint32_t task_id) {
    return 0;
}
