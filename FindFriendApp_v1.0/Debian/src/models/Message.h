#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <time.h>

typedef enum {
    MSG_TYPE_TEXT = 0,
    MSG_TYPE_IMAGE = 1,
    MSG_TYPE_AUDIO = 2,
    MSG_TYPE_VIDEO = 3,
    MSG_TYPE_FILE = 4,
    MSG_TYPE_EXHIBITION = 5,  // 展会信息
    MSG_TYPE_TICKET = 6       // 票务信息
} MessageType;

typedef struct {
    uint32_t id;
    uint32_t from_id;
    uint32_t to_id;
    uint32_t group_id;         // 群组ID，0为私聊
    MessageType type;
    char content[4096];
    char media_path[512];      // 媒体文件路径
    char thumbnail[512];       // 缩略图
    time_t timestamp;
    int is_read;
    int is_official;           // 官方消息
    int is_system;             // 系统消息
} Message;

// 免打扰设置
typedef struct {
    uint32_t user_id;
    int global_do_not_disturb;  // 全局免打扰
    int official_only;           // 仅官方消息
    uint32_t muted_groups[50];   // 静音的群组
    int muted_count;
} DoNotDisturb;

#endif
