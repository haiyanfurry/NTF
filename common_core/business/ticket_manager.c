#include "ticket_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 生成随机入场码
static char* generate_ticket_code() {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int code_length = 12;
    char* code = (char*)malloc(code_length + 1);
    if (!code) {
        return NULL;
    }
    
    for (int i = 0; i < code_length; i++) {
        code[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    code[code_length] = '\0';
    
    return code;
}

// 入场码响应回调函数
static void ticket_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    TicketCallback callback = (TicketCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    TicketManager* manager = (TicketManager*)((void**)user_data)[2];
    const char* action = (const char*)((void**)user_data)[3];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 根据操作类型处理响应
        if (strcmp(action, "generate_ticket") == 0) {
            // 模拟生成入场码
            if (manager->tickets) {
                free(manager->tickets);
            }
            manager->ticket_count = 1;
            manager->tickets = (Ticket*)malloc(sizeof(Ticket) * manager->ticket_count);
            if (manager->tickets) {
                char* code = generate_ticket_code();
                if (code) {
                    manager->tickets[0].ticket_id = strdup("ticket_123");
                    manager->tickets[0].code = code;
                    manager->tickets[0].type = TICKET_TYPE_EXHIBITION;
                    manager->tickets[0].status = TICKET_STATUS_VALID;
                    manager->tickets[0].user_id = strdup("user_123");
                    manager->tickets[0].exhibition_id = strdup("exhibition_123");
                    manager->tickets[0].event_id = strdup("event_123");
                    
                    // 生成时间
                    time_t now = time(NULL);
                    struct tm* tm_info = localtime(&now);
                    char time_str[20];
                    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                    manager->tickets[0].create_time = strdup(time_str);
                    
                    // 生成过期时间（24小时后）
                    now += 24 * 3600;
                    tm_info = localtime(&now);
                    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                    manager->tickets[0].expire_time = strdup(time_str);
                    
                    manager->tickets[0].used_time = NULL;
                    manager->tickets[0].qr_code_url = strdup("http://example.com/qr/ticket_123");
                }
            }
        } else if (strcmp(action, "validate_ticket") == 0) {
            printf("Ticket validated successfully\n");
        } else if (strcmp(action, "use_ticket") == 0) {
            printf("Ticket used successfully\n");
        } else if (strcmp(action, "get_user_tickets") == 0) {
            // 模拟用户入场码列表
            if (manager->tickets) {
                free(manager->tickets);
            }
            manager->ticket_count = 2;
            manager->tickets = (Ticket*)malloc(sizeof(Ticket) * manager->ticket_count);
            if (manager->tickets) {
                // 入场码 1
                manager->tickets[0].ticket_id = strdup("ticket_123");
                manager->tickets[0].code = strdup("ABCDEFG123456");
                manager->tickets[0].type = TICKET_TYPE_EXHIBITION;
                manager->tickets[0].status = TICKET_STATUS_VALID;
                manager->tickets[0].user_id = strdup("user_123");
                manager->tickets[0].exhibition_id = strdup("exhibition_123");
                manager->tickets[0].event_id = strdup("event_123");
                manager->tickets[0].create_time = strdup("2026-04-01 10:00:00");
                manager->tickets[0].expire_time = strdup("2026-04-02 10:00:00");
                manager->tickets[0].used_time = NULL;
                manager->tickets[0].qr_code_url = strdup("http://example.com/qr/ticket_123");
                
                // 入场码 2
                manager->tickets[1].ticket_id = strdup("ticket_456");
                manager->tickets[1].code = strdup("XYZ123ABC456");
                manager->tickets[1].type = TICKET_TYPE_VIP;
                manager->tickets[1].status = TICKET_STATUS_USED;
                manager->tickets[1].user_id = strdup("user_123");
                manager->tickets[1].exhibition_id = strdup("exhibition_456");
                manager->tickets[1].event_id = strdup("event_456");
                manager->tickets[1].create_time = strdup("2026-03-30 14:00:00");
                manager->tickets[1].expire_time = strdup("2026-03-31 14:00:00");
                manager->tickets[1].used_time = strdup("2026-03-31 10:30:00");
                manager->tickets[1].qr_code_url = strdup("http://example.com/qr/ticket_456");
            }
        } else if (strcmp(action, "get_exhibition_tickets") == 0) {
            printf("Exhibition tickets retrieved successfully\n");
        } else if (strcmp(action, "check_ticket_status") == 0) {
            printf("Ticket status checked successfully\n");
        } else if (strcmp(action, "batch_generate_tickets") == 0) {
            printf("Batch tickets generated successfully\n");
        }
        
        // 调用回调函数
        if (callback) {
            callback(callback_user_data, status, manager->tickets);
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

// 初始化入场码管理器
TicketManager* ticket_manager_init() {
    TicketManager* manager = (TicketManager*)malloc(sizeof(TicketManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->tickets = NULL;
    manager->ticket_count = 0;
    
    // 初始化随机数种子
    srand(time(NULL));
    
    return manager;
}

// 销毁入场码管理器
void ticket_manager_destroy(TicketManager* manager) {
    if (manager) {
        // 释放入场码列表
        if (manager->tickets) {
            for (size_t i = 0; i < manager->ticket_count; i++) {
                if (manager->tickets[i].ticket_id) free(manager->tickets[i].ticket_id);
                if (manager->tickets[i].code) free(manager->tickets[i].code);
                if (manager->tickets[i].user_id) free(manager->tickets[i].user_id);
                if (manager->tickets[i].exhibition_id) free(manager->tickets[i].exhibition_id);
                if (manager->tickets[i].event_id) free(manager->tickets[i].event_id);
                if (manager->tickets[i].create_time) free(manager->tickets[i].create_time);
                if (manager->tickets[i].expire_time) free(manager->tickets[i].expire_time);
                if (manager->tickets[i].used_time) free(manager->tickets[i].used_time);
                if (manager->tickets[i].qr_code_url) free(manager->tickets[i].qr_code_url);
            }
            free(manager->tickets);
        }
        free(manager);
    }
}

// 生成入场码
bool ticket_manager_generate_ticket(TicketManager* manager, const char* user_id, const char* exhibition_id, const char* event_id, int ticket_type, TicketCallback callback, void* user_data) {
    if (!manager || !user_id || !exhibition_id || !event_id) {
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
    callback_data[3] = (void*)strdup("generate_ticket");
    
    // 构建入场码数据
    char* data = (char*)malloc(1024);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 1024, "{\"user_id\": \"%s\", \"exhibition_id\": \"%s\", \"event_id\": \"%s\", \"ticket_type\": %d}", 
             user_id, exhibition_id, event_id, ticket_type);
    
    // 模拟生成入场码请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Generating ticket for user: %s, exhibition: %s\n", user_id, exhibition_id);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 验证入场码
bool ticket_manager_validate_ticket(TicketManager* manager, const char* ticket_code, TicketCallback callback, void* user_data) {
    if (!manager || !ticket_code) {
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
    callback_data[3] = (void*)strdup("validate_ticket");
    
    // 构建验证请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"ticket_code\": \"%s\"}", ticket_code);
    
    // 模拟验证入场码请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Validating ticket: %s\n", ticket_code);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 使用入场码
bool ticket_manager_use_ticket(TicketManager* manager, const char* ticket_code, TicketCallback callback, void* user_data) {
    if (!manager || !ticket_code) {
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
    callback_data[3] = (void*)strdup("use_ticket");
    
    // 构建使用请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"ticket_code\": \"%s\"}", ticket_code);
    
    // 模拟使用入场码请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Using ticket: %s\n", ticket_code);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 获取用户的入场码列表
bool ticket_manager_get_user_tickets(TicketManager* manager, const char* user_id, TicketCallback callback, void* user_data) {
    if (!manager || !user_id) {
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
    callback_data[3] = (void*)strdup("get_user_tickets");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"user_id\": \"%s\"}", user_id);
    
    // 模拟获取用户入场码列表请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting tickets for user: %s\n", user_id);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 获取展会的入场码列表
bool ticket_manager_get_exhibition_tickets(TicketManager* manager, const char* exhibition_id, TicketCallback callback, void* user_data) {
    if (!manager || !exhibition_id) {
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
    callback_data[3] = (void*)strdup("get_exhibition_tickets");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"exhibition_id\": \"%s\"}", exhibition_id);
    
    // 模拟获取展会入场码列表请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Getting tickets for exhibition: %s\n", exhibition_id);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 检查入场码状态
bool ticket_manager_check_ticket_status(TicketManager* manager, const char* ticket_code, TicketCallback callback, void* user_data) {
    if (!manager || !ticket_code) {
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
    callback_data[3] = (void*)strdup("check_ticket_status");
    
    // 构建请求数据
    char* data = (char*)malloc(256);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 256, "{\"ticket_code\": \"%s\"}", ticket_code);
    
    // 模拟检查入场码状态请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Checking ticket status: %s\n", ticket_code);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}

// 批量生成入场码
bool ticket_manager_batch_generate_tickets(TicketManager* manager, const char* user_id, const char* exhibition_id, const char* event_id, int ticket_type, int count, TicketCallback callback, void* user_data) {
    if (!manager || !user_id || !exhibition_id || !event_id || count <= 0) {
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
    callback_data[3] = (void*)strdup("batch_generate_tickets");
    
    // 构建批量生成请求数据
    char* data = (char*)malloc(1024);
    if (!data) {
        fprintf(stderr, "Error allocating memory\n");
        free((void*)callback_data[3]);
        free(callback_data);
        return false;
    }
    
    snprintf(data, 1024, "{\"user_id\": \"%s\", \"exhibition_id\": \"%s\", \"event_id\": \"%s\", \"ticket_type\": %d, \"count\": %d}", 
             user_id, exhibition_id, event_id, ticket_type, count);
    
    // 模拟批量生成入场码请求
    // 实际应用中应该使用 network_manager 发送请求
    printf("Batch generating %d tickets for user: %s, exhibition: %s\n", count, user_id, exhibition_id);
    
    // 模拟请求成功
    ticket_response_callback(callback_data, 200, NULL, 0);
    
    free(data);
    return true;
}
