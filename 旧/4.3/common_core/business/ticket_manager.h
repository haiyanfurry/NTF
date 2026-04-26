#ifndef TICKET_MANAGER_H
#define TICKET_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 入场码状态
#define TICKET_STATUS_VALID 0      // 有效
#define TICKET_STATUS_USED 1       // 已使用
#define TICKET_STATUS_EXPIRED 2    // 已过期
#define TICKET_STATUS_INVALID 3    // 无效

// 入场码类型
#define TICKET_TYPE_EXHIBITION 0   // 展会票
#define TICKET_TYPE_EVENT 1        // 活动票
#define TICKET_TYPE_VIP 2          // VIP票

// 入场码结构
typedef struct {
    char* ticket_id;            // 入场码ID
    char* code;                 // 入场码
    int type;                   // 入场码类型
    int status;                 // 入场码状态
    char* user_id;              // 用户ID
    char* exhibition_id;        // 展会ID
    char* event_id;             // 活动ID
    char* create_time;          // 创建时间
    char* expire_time;          // 过期时间
    char* used_time;            // 使用时间
    char* qr_code_url;          // 二维码URL
} Ticket;

// 入场码管理器
typedef struct {
    void* network_manager;      // 网络管理器
    Ticket* tickets;            // 入场码列表
    size_t ticket_count;        // 入场码数量
} TicketManager;

// 入场码回调函数类型
typedef void (*TicketCallback)(void* user_data, int status, Ticket* tickets);

// 初始化入场码管理器
TicketManager* ticket_manager_init();

// 销毁入场码管理器
void ticket_manager_destroy(TicketManager* manager);

// 生成入场码
bool ticket_manager_generate_ticket(TicketManager* manager, const char* user_id, const char* exhibition_id, const char* event_id, int ticket_type, TicketCallback callback, void* user_data);

// 验证入场码
bool ticket_manager_validate_ticket(TicketManager* manager, const char* ticket_code, TicketCallback callback, void* user_data);

// 使用入场码
bool ticket_manager_use_ticket(TicketManager* manager, const char* ticket_code, TicketCallback callback, void* user_data);

// 获取用户的入场码列表
bool ticket_manager_get_user_tickets(TicketManager* manager, const char* user_id, TicketCallback callback, void* user_data);

// 获取展会的入场码列表
bool ticket_manager_get_exhibition_tickets(TicketManager* manager, const char* exhibition_id, TicketCallback callback, void* user_data);

// 检查入场码状态
bool ticket_manager_check_ticket_status(TicketManager* manager, const char* ticket_code, TicketCallback callback, void* user_data);

// 批量生成入场码
bool ticket_manager_batch_generate_tickets(TicketManager* manager, const char* user_id, const char* exhibition_id, const char* event_id, int ticket_type, int count, TicketCallback callback, void* user_data);

#endif // TICKET_MANAGER_H
