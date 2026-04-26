// ====================================================================
// FindFriend 消息系统接口
// 业务模块：消息发送、接收、管理
// ====================================================================

#ifndef FF_MESSAGE_INTERFACE_H
#define FF_MESSAGE_INTERFACE_H

#include "ff_core_interface.h"
#include <stdint.h>

// ====================================================================
// 消息类型
// ====================================================================
typedef enum FFMessageType {
    FF_MSG_TEXT = 1,
    FF_MSG_IMAGE = 2,
    FF_MSG_VOICE = 3,
    FF_MSG_VIDEO = 4,
    FF_MSG_LOCATION = 5,
    FF_MSG_FILE = 6,
    FF_MSG_SYSTEM = 7
} FFMessageType;

typedef enum FFMessageStatus {
    FF_MSG_SENDING = 0,
    FF_MSG_SENT = 1,
    FF_MSG_DELIVERED = 2,
    FF_MSG_READ = 3,
    FF_MSG_FAILED = 4
} FFMessageStatus;

// ====================================================================
// 消息结构
// ====================================================================
typedef struct FFMessage {
    uint64_t msg_id;
    uint32_t from_uid;
    uint32_t to_uid;
    FFMessageType type;
    FFMessageStatus status;
    char content[4096];
    char media_url[512];
    double latitude;
    double longitude;
    uint64_t timestamp;
    bool is_deleted;
} FFMessage;

// ====================================================================
// 会话结构
// ====================================================================
typedef struct FFConversation {
    uint32_t peer_uid;
    char peer_name[64];
    FFMessage* last_message;
    uint32_t unread_count;
    uint64_t updated_at;
} FFConversation;

// ====================================================================
// 消息管理接口
// ====================================================================

// 发送消息
FFResult ff_message_send(uint32_t to_uid, FFMessageType type, const char* content, FFMessage** out_msg, FFAsyncCallback callback, void* user_data);
FFResult ff_message_send_text(uint32_t to_uid, const char* text, FFMessage** out_msg, FFAsyncCallback callback, void* user_data);
FFResult ff_message_send_image(uint32_t to_uid, const char* image_path, FFMessage** out_msg, FFAsyncCallback callback, void* user_data);
FFResult ff_message_send_location(uint32_t to_uid, double lat, double lon, FFMessage** out_msg, FFAsyncCallback callback, void* user_data);

// 接收消息
FFResult ff_message_receive(FFMessage** messages, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_message_get(uint64_t msg_id, FFMessage** message, FFAsyncCallback callback, void* user_data);

// 消息状态
FFResult ff_message_mark_read(uint64_t msg_id, FFAsyncCallback callback, void* user_data);
FFResult ff_message_mark_all_read(uint32_t from_uid, FFAsyncCallback callback, void* user_data);
FFResult ff_message_delete(uint64_t msg_id, FFAsyncCallback callback, void* user_data);

// 会话管理
FFResult ff_conversation_get_list(FFConversation** conversations, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_conversation_get_unread_count(uint32_t uid, uint32_t* count);
FFResult ff_conversation_clear_unread(uint32_t uid);

// 消息搜索
FFResult ff_message_search(const char* keyword, FFMessage** messages, size_t* count, FFAsyncCallback callback, void* user_data);

// 内存释放
void ff_message_free(FFMessage* message);
void ff_message_list_free(FFMessage* messages, size_t count);
void ff_conversation_list_free(FFConversation* conversations, size_t count);

#endif // FF_MESSAGE_INTERFACE_H
