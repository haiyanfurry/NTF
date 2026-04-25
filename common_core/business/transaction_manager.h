#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 交易状态
#define TRANSACTION_STATUS_PENDING 0      // 待审核
#define TRANSACTION_STATUS_APPROVED 1     // 已通过
#define TRANSACTION_STATUS_REJECTED 2     // 已拒绝
#define TRANSACTION_STATUS_COMPLETED 3    // 已完成
#define TRANSACTION_STATUS_CANCELLED 4    // 已取消

// 交易类型
#define TRANSACTION_TYPE_ARTICLE 0        // 稿件
#define TRANSACTION_TYPE_TICKET 1         // 门票
#define TRANSACTION_TYPE_MERCHANDISE 2    //  merchandise

// 交易结构
typedef struct {
    char* transaction_id;        // 交易ID
    char* seller_id;             // 卖家ID
    char* buyer_id;              // 买家ID（可选）
    int type;                    // 交易类型
    int status;                  // 交易状态
    char* item_id;               // 商品ID
    char* item_name;             // 商品名称
    float price;                 // 价格
    int quantity;                // 数量
    char* description;           // 描述
    char* create_time;           // 创建时间
    char* approve_time;          // 审核时间
    char* complete_time;         // 完成时间
    char* reject_reason;         // 拒绝原因
    char*审核员ID;              // 审核员ID
} Transaction;

// 交易管理器
typedef struct {
    void* network_manager;      // 网络管理器
    Transaction* transactions;   // 交易列表
    size_t transaction_count;    // 交易数量
} TransactionManager;

// 交易回调函数类型
typedef void (*TransactionCallback)(void* user_data, int status, Transaction* transactions);

// 初始化交易管理器
TransactionManager* transaction_manager_init();

// 销毁交易管理器
void transaction_manager_destroy(TransactionManager* manager);

// 创建交易（卖家发布商品）
bool transaction_manager_create_transaction(TransactionManager* manager, const char* seller_id, int type, const char* item_name, float price, int quantity, const char* description, TransactionCallback callback, void* user_data);

// 获取待审核交易列表
bool transaction_manager_get_pending_transactions(TransactionManager* manager, TransactionCallback callback, void* user_data);

// 审核交易
bool transaction_manager_audit_transaction(TransactionManager* manager, const char* transaction_id, int status, const char* reason, const char*审核员ID, TransactionCallback callback, void* user_data);

// 获取用户的交易列表
bool transaction_manager_get_user_transactions(TransactionManager* manager, const char* user_id, TransactionCallback callback, void* user_data);

// 获取交易详情
bool transaction_manager_get_transaction_detail(TransactionManager* manager, const char* transaction_id, TransactionCallback callback, void* user_data);

// 取消交易
bool transaction_manager_cancel_transaction(TransactionManager* manager, const char* transaction_id, TransactionCallback callback, void* user_data);

// 完成交易
bool transaction_manager_complete_transaction(TransactionManager* manager, const char* transaction_id, TransactionCallback callback, void* user_data);

// 搜索交易
bool transaction_manager_search_transactions(TransactionManager* manager, const char* keyword, int type, int status, TransactionCallback callback, void* user_data);

#endif // TRANSACTION_MANAGER_H
