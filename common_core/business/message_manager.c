// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "message_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 消息响应回调函数
static void message_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    MessageCallback callback = (MessageCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    MessageManager* manager = (MessageManager*)((void**)user_data)[2];
    const char* action = (const char*)((void**)user_data)[3];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 根据操作类型处理响应
        if (strcmp(action, "send_message") == 0) {
            printf("Message sent successfully\n");
        } else if (strcmp(action, "get_messages") == 0) {
            // 模拟消息列表
            if (manager->messages) {
                free(manager->messages);
                manager->messages = NULL;
            }
            manager->message_count = 3;
            manager->messages = (Message*)malloc(sizeof(Message) * manager->message_count);
            if (!manager->messages) {
                fprintf(stderr, "Error allocating memory for messages\n");
                return;
            }
            if (manager->messages) {
                // 消息 1
                manager->messages[0].id = 1;
                manager->messages[0].from_id = 123;
                manager->messages[0].to_id = 456;
                manager->messages[0].group_id = 0;
                manager->messages[0].type = MSG_TYPE_TEXT;
                strcpy(manager->messages[0].content, "Hello, how are you?");
                manager->messages[0].media_path[0] = '\0';
                manager->messages[0].thumbnail[0] = '\0';
                manager->messages[0].timestamp = time(NULL) - 3600;
                manager->messages[0].is_read = 0;
                manager->messages[0].is_official = 0;
                manager->messages[0].is_system = 0;
                
                // 消息 2
                manager->messages[1].id = 2;
                manager->messages[1].from_id = 456;
                manager->messages[1].to_id = 123;
                manager->messages[1].group_id = 0;
                manager->messages[1].type = MSG_TYPE_TEXT;
                strcpy(manager->messages[1].content, "I'm fine, thank you!");
                manager->messages[1].media_path[0] = '\0';
                manager->messages[1].thumbnail[0] = '\0';
                manager->messages[1].timestamp = time(NULL) - 3000;
                manager->messages[1].is_read = 1;
                manager->messages[1].is_official = 0;
                manager->messages[1].is_system = 0;
                
                // 消息 3
                manager->messages[2].id = 3;
                manager->messages[2].from_id = 0;
                manager->messages[2].to_id = 123;
                manager->messages[2].group_id = 0;
                manager->messages[2].type = MSG_TYPE_EXHIBITION;
                strcpy(manager->messages[2].content, "您关注的展会即将开始，点击查看详情");
                manager->messages[2].media_path[0] = '\0';
                manager->messages[2].thumbnail[0] = '\0';
                manager->messages[2].timestamp = time(NULL) - 600;
                manager->messages[2].is_read = 0;
                manager->messages[2].is_official = 1;
                manager->messages[2].is_system = 1;
            }
        } else if (strcmp(action, "mark_read") == 0) {
            printf("Message marked as read\n");
        } else if (strcmp(action, "mark_all_read") == 0) {
            printf("All messages marked as read\n");
        } else if (strcmp(action, "delete_message") == 0) {
            printf("Message deleted\n");
        } else if (strcmp(action, "set_dnd") == 0) {
            printf("Do not disturb settings updated\n");
        } else if (strcmp(action, "mute_group") == 0) {
            printf("Group mute status updated\n");
        }
        
        // 调用回调函数
        if (callback) {
            callback(callback_user_data, status, manager->messages);
        }
    } else {
        // 操作失败
        if (callback) {
            callback(callback_user_data, status, NULL);
        }
    }
    
    // 清理资源
    free((void*)action);
    free(user_data);
}

// 初始化消息管理器
MessageManager* message_manager_init() {
    MessageManager* manager = (MessageManager*)malloc(sizeof(MessageManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->messages = NULL;
    manager->message_count = 0;
    manager->push_enabled = true;
    
    // 初始化免打扰设置
    manager->dnd_settings.user_id = 0;
    manager->dnd_settings.global_do_not_disturb = 0;
    manager->dnd_settings.official_only = 0;
    manager->dnd_settings.muted_count = 0;
    memset(manager->dnd_settings.muted_groups, 0, sizeof(manager->dnd_settings.muted_groups));
    
    return manager;
}

// 销毁消息管理器
void message_manager_destroy(MessageManager* manager) {
    if (manager) {
        // 释放消息列表
        if (manager->messages) {
            free(manager->messages);
            manager->messages = NULL;
        }
        free(manager);
    }
}

// 发送消息
bool message_manager_send_message(MessageManager* manager, uint32_t to_id, uint32_t group_id, MessageType type, const char* content, const char* media_path, MessageCallback callback, void* user_data) {
    if (!manager || !content) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("send_message");
    
    // 构建消息数据
    char* data = (char*)malloc(5000);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 5000, "{\"to_id\": %u, \"group_id\": %u, \"type\": %d, \"content\": \"%s\", \"media_path\": \"%s\"}", 
             to_id, group_id, type, content, media_path ? media_path : "");
    
    // 模拟发送消息请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Sending message to %u, group %u\n", to_id, group_id);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 接收消息
bool message_manager_receive_message(MessageManager* manager, Message* message) {
    if (!manager || !message) {
        return false;
    }
    
    // 检查是否免打扰
    if (message_manager_is_do_not_disturb(manager, message->from_id, message->group_id, message->is_official)) {
        // 免打扰状态，不显示通知
        printf("Message received but do not disturb is enabled\n");
    } else {
        // 显示通知
        printf("Message received from %u: %s\n", message->from_id, message->content);
    }
    
    // 官方消息自动标记已读
    if (message->is_official) {
        message->is_read = 1;
    }
    
    // 添加到消息列表
    Message* new_messages = (Message*)realloc(manager->messages, sizeof(Message) * (manager->message_count + 1));
    if (!new_messages) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    manager->messages = new_messages;
    manager->messages[manager->message_count] = *message;
    manager->message_count++;
    
    return true;
}

// 获取消息列表
bool message_manager_get_messages(MessageManager* manager, uint32_t user_id, uint32_t group_id, int count, int offset, MessageCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("get_messages");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"user_id\": %u, \"group_id\": %u, \"count\": %d, \"offset\": %d}", 
             user_id, group_id, count, offset);
    
    // 模拟获取消息列表请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting messages for user %u, group %u\n", user_id, group_id);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 标记消息已读
bool message_manager_mark_message_read(MessageManager* manager, uint32_t message_id, MessageCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("mark_read");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"message_id\": %u}", message_id);
    
    // 模拟标记已读请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Marking message %u as read\n", message_id);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 标记所有消息已读
bool message_manager_mark_all_read(MessageManager* manager, uint32_t user_id, uint32_t group_id, MessageCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("mark_all_read");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"user_id\": %u, \"group_id\": %u}", user_id, group_id);
    
    // 模拟标记所有已读请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Marking all messages as read for user %u, group %u\n", user_id, group_id);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 删除消息
bool message_manager_delete_message(MessageManager* manager, uint32_t message_id, MessageCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("delete_message");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"message_id\": %u}", message_id);
    
    // 模拟删除消息请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Deleting message %u\n", message_id);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 设置免打扰
bool message_manager_set_do_not_disturb(MessageManager* manager, int global_dnd, int official_only, MessageCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 更新免打扰设置
    manager->dnd_settings.global_do_not_disturb = global_dnd;
    manager->dnd_settings.official_only = official_only;
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("set_dnd");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"global_dnd\": %d, \"official_only\": %d}", global_dnd, official_only);
    
    // 模拟设置免打扰请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Setting do not disturb: global=%d, official_only=%d\n", global_dnd, official_only);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 静音群组
bool message_manager_mute_group(MessageManager* manager, uint32_t group_id, bool mute, MessageCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 检查群组是否已在静音列表中
    int index = -1;
    for (int i = 0; i < manager->dnd_settings.muted_count; i++) {
        if (manager->dnd_settings.muted_groups[i] == group_id) {
            index = i;
            break;
        }
    }
    
    if (mute) {
        // 添加到静音列表
        if (index == -1 && manager->dnd_settings.muted_count < 50) {
            manager->dnd_settings.muted_groups[manager->dnd_settings.muted_count++] = group_id;
        }
    } else {
        // 从静音列表中移除
        if (index != -1) {
            for (int i = index; i < manager->dnd_settings.muted_count - 1; i++) {
                manager->dnd_settings.muted_groups[i] = manager->dnd_settings.muted_groups[i + 1];
            }
            manager->dnd_settings.muted_count--;
        }
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 4);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    callback_data[3] = (void*)strdup("mute_group");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"group_id\": %u, \"mute\": %d}", group_id, mute ? 1 : 0);
    
    // 模拟静音群组请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("%s group %u\n", mute ? "Muting" : "Unmuting", group_id);
    
    // 模拟请求成功
    message_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 检查是否免打扰
bool message_manager_is_do_not_disturb(MessageManager* manager, uint32_t sender_id, uint32_t group_id, int is_official) {
    if (!manager) {
        return false;
    }
    
    // 全局免打扰
    if (manager->dnd_settings.global_do_not_disturb) {
        // 仅官方消息可推送
        if (manager->dnd_settings.official_only) {
            return !is_official;
        }
        return true;
    }
    
    // 群组静音
    if (group_id > 0) {
        for (int i = 0; i < manager->dnd_settings.muted_count; i++) {
            if (manager->dnd_settings.muted_groups[i] == group_id) {
                return true;
            }
        }
    }
    
    return false;
}

// 启用/禁用推送
bool message_manager_set_push_enabled(MessageManager* manager, bool enabled) {
    if (!manager) {
        return false;
    }
    
    manager->push_enabled = enabled;
    printf("Push %s\n", enabled ? "enabled" : "disabled");
    
    return true;
}

// 获取未读消息数
size_t message_manager_get_unread_count(MessageManager* manager, uint32_t user_id, uint32_t group_id) {
    if (!manager) {
        return 0;
    }
    
    size_t count = 0;
    for (size_t i = 0; i < manager->message_count; i++) {
        if (manager->messages[i].to_id == user_id && 
            (group_id == 0 || manager->messages[i].group_id == group_id) && 
            manager->messages[i].is_read == 0) {
            count++;
        }
    }
    
    return count;
}

// 清理消息
bool message_manager_cleanup_messages(MessageManager* manager, uint32_t user_id, uint32_t group_id, time_t before_time) {
    if (!manager) {
        return false;
    }
    
    // 统计需要保留的消息数量
    size_t keep_count = 0;
    for (size_t i = 0; i < manager->message_count; i++) {
        if (manager->messages[i].to_id == user_id && 
            (group_id == 0 || manager->messages[i].group_id == group_id) && 
            manager->messages[i].timestamp >= before_time) {
            keep_count++;
        }
    }
    
    // 创建新的消息列表
    Message* new_messages = (Message*)malloc(sizeof(Message) * keep_count);
    if (!new_messages) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    // 复制需要保留的消息
    size_t index = 0;
    for (size_t i = 0; i < manager->message_count; i++) {
        if (manager->messages[i].to_id == user_id && 
            (group_id == 0 || manager->messages[i].group_id == group_id) && 
            manager->messages[i].timestamp >= before_time) {
            new_messages[index++] = manager->messages[i];
        }
    }
    
    // 释放旧消息列表
    if (manager->messages) {
        free(manager->messages);
        manager->messages = NULL;
    }
    
    // 更新消息列表
    manager->messages = new_messages;
    manager->message_count = keep_count;
    
    printf("Cleaned up messages before %ld, kept %zu messages\n", before_time, keep_count);
    
    return true;
}
