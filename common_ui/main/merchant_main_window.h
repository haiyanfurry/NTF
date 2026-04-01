#ifndef MERCHANT_MAIN_WINDOW_H
#define MERCHANT_MAIN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

// 商家端主窗口结构体
typedef struct {
    void* native_window;
    void* product_management;
    void* order_management;
    void* transaction_audit;
    void* exhibition_management;
    void* ticket_management;
    void* sales_statistics;
    void* user_info;
} MerchantMainWindow;

// 创建商家端主窗口
MerchantMainWindow* merchant_main_window_create(void);

// 销毁商家端主窗口
void merchant_main_window_destroy(MerchantMainWindow* window);

// 显示商家端主窗口
void merchant_main_window_show(MerchantMainWindow* window);

// 隐藏商家端主窗口
void merchant_main_window_hide(MerchantMainWindow* window);

// 设置用户信息
void merchant_main_window_set_user_info(MerchantMainWindow* window, const char* username, const char* nickname, const char* avatar);

// 切换到商品管理
void merchant_main_window_switch_to_products(MerchantMainWindow* window);

// 切换到订单管理
void merchant_main_window_switch_to_orders(MerchantMainWindow* window);

// 切换到交易审核
void merchant_main_window_switch_to_transaction_audit(MerchantMainWindow* window);

// 切换到展会管理
void merchant_main_window_switch_to_exhibition(MerchantMainWindow* window);

// 切换到门票管理
void merchant_main_window_switch_to_ticket(MerchantMainWindow* window);

// 切换到销售统计
void merchant_main_window_switch_to_sales_statistics(MerchantMainWindow* window);

// 切换到用户信息
void merchant_main_window_switch_to_user_info(MerchantMainWindow* window);

#endif // MERCHANT_MAIN_WINDOW_H
