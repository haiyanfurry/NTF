#include "merchant_main_window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建商家端主窗口
MerchantMainWindow* merchant_main_window_create(void) {
    MerchantMainWindow* window = (MerchantMainWindow*)malloc(sizeof(MerchantMainWindow));
    if (!window) {
        fprintf(stderr, "Error allocating memory for merchant main window\n");
        return NULL;
    }
    
    // 初始化窗口组件
    window->native_window = NULL; // 实际应用中应该创建原生窗口
    window->product_management = NULL;
    window->order_management = NULL;
    window->transaction_audit = NULL;
    window->exhibition_management = NULL;
    window->ticket_management = NULL;
    window->sales_statistics = NULL;
    window->user_info = NULL;
    
    printf("Merchant main window created\n");
    return window;
}

// 销毁商家端主窗口
void merchant_main_window_destroy(MerchantMainWindow* window) {
    if (window) {
        // 释放窗口组件
        // 实际应用中应该销毁原生窗口和其他组件
        free(window);
        printf("Merchant main window destroyed\n");
    }
}

// 显示商家端主窗口
void merchant_main_window_show(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window shown\n");
    }
}

// 隐藏商家端主窗口
void merchant_main_window_hide(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window hidden\n");
    }
}

// 设置用户信息
void merchant_main_window_set_user_info(MerchantMainWindow* window, const char* username, const char* nickname, const char* avatar) {
    if (window) {
        printf("Merchant main window user info set: username=%s, nickname=%s, avatar=%s\n", username, nickname, avatar);
    }
}

// 切换到商品管理
void merchant_main_window_switch_to_products(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to product management\n");
    }
}

// 切换到订单管理
void merchant_main_window_switch_to_orders(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to order management\n");
    }
}

// 切换到交易审核
void merchant_main_window_switch_to_transaction_audit(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to transaction audit\n");
    }
}

// 切换到展会管理
void merchant_main_window_switch_to_exhibition(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to exhibition management\n");
    }
}

// 切换到门票管理
void merchant_main_window_switch_to_ticket(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to ticket management\n");
    }
}

// 切换到销售统计
void merchant_main_window_switch_to_sales_statistics(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to sales statistics\n");
    }
}

// 切换到用户信息
void merchant_main_window_switch_to_user_info(MerchantMainWindow* window) {
    if (window) {
        printf("Merchant main window switched to user info\n");
    }
}
