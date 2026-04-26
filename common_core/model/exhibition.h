#ifndef EXHIBITION_H
#define EXHIBITION_H

#include <stdint.h>
#include <time.h>

typedef enum {
    TICKET_STATUS_PENDING = 0,    // 待审核
    TICKET_STATUS_APPROVED = 1,   // 已审核
    TICKET_STATUS_SOLD = 2,       // 已售出
    TICKET_STATUS_USED = 3        // 已使用
} TicketStatus;

typedef struct {
    uint32_t id;
    char name[128];
    char description[512];
    char poster_url[256];
    time_t start_time;
    time_t end_time;
    char location[256];
    double latitude;
    double longitude;
    int ticket_count;              // 票务总数
    int ticket_sold;               // 已售数量
    int price;                     // 价格(分)
    int is_official;               // 是否官方展会
    char organizer[128];           // 主办方
} Exhibition;

typedef struct {
    uint32_t id;
    uint32_t exhibition_id;
    uint32_t user_id;
    char qr_code[256];             // 入场码
    TicketStatus status;
    time_t purchase_time;
    time_t use_time;
    char verification_code[32];    // 核验码
} Ticket;

typedef struct {
    uint32_t id;
    uint32_t seller_id;
    uint32_t buyer_id;
    char item_name[128];
    char item_type[32];            // 稿件/门票/周边
    int price;
    char description[512];
    int status;                     // 0:待审核 1:已审核 2:已拒绝 3:已完成
    char review_comment[256];
    time_t submit_time;
    time_t review_time;
} Transaction;

#endif
