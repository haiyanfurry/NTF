#include "MerchantDashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QGroupBox>

MerchantDashboard::MerchantDashboard(QWidget *parent) : QWidget(parent) {
    m_mainLayout = new QVBoxLayout(this);
    setupDashboard();
    setLayout(m_mainLayout);
}

MerchantDashboard::~MerchantDashboard() {
}

void MerchantDashboard::setupDashboard() {
    // 标题
    QLabel *titleLabel = new QLabel("商家仪表盘");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    
    // 刷新按钮
    m_refreshButton = new QPushButton("刷新数据");
    m_mainLayout->addWidget(m_refreshButton);
    connect(m_refreshButton, &QPushButton::clicked, this, &MerchantDashboard::onRefreshClicked);
    
    // 统计信息
    setupStats();
    
    // 最近订单
    setupRecentOrders();
    
    // 快速操作
    setupQuickActions();
}

void MerchantDashboard::setupStats() {
    QGroupBox *statsGroup = new QGroupBox("业务统计");
    QGridLayout *statsLayout = new QGridLayout(statsGroup);
    
    // 销售额
    QVBoxLayout *salesLayout = new QVBoxLayout();
    QLabel *salesTitle = new QLabel("总销售额");
    m_salesLabel = new QLabel("¥12,345.67");
    m_salesLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4CAF50;");
    salesLayout->addWidget(salesTitle);
    salesLayout->addWidget(m_salesLabel);
    statsLayout->addLayout(salesLayout, 0, 0);
    
    // 订单数
    QVBoxLayout *ordersLayout = new QVBoxLayout();
    QLabel *ordersTitle = new QLabel("订单数");
    m_ordersLabel = new QLabel("123");
    m_ordersLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2196F3;");
    ordersLayout->addWidget(ordersTitle);
    ordersLayout->addWidget(m_ordersLabel);
    statsLayout->addLayout(ordersLayout, 0, 1);
    
    // 客户数
    QVBoxLayout *customersLayout = new QVBoxLayout();
    QLabel *customersTitle = new QLabel("客户数");
    m_customersLabel = new QLabel("45");
    m_customersLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF9800;");
    customersLayout->addWidget(customersTitle);
    customersLayout->addWidget(m_customersLabel);
    statsLayout->addLayout(customersLayout, 0, 2);
    
    m_mainLayout->addWidget(statsGroup);
}

void MerchantDashboard::setupRecentOrders() {
    QGroupBox *ordersGroup = new QGroupBox("最近订单");
    QVBoxLayout *ordersLayout = new QVBoxLayout(ordersGroup);
    
    m_recentOrdersTable = new QTableWidget();
    m_recentOrdersTable->setColumnCount(4);
    m_recentOrdersTable->setHorizontalHeaderLabels({"订单号", "客户", "金额", "状态"});
    m_recentOrdersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 添加示例数据
    m_recentOrdersTable->setRowCount(5);
    m_recentOrdersTable->setItem(0, 0, new QTableWidgetItem("ORD-001"));
    m_recentOrdersTable->setItem(0, 1, new QTableWidgetItem("张三"));
    m_recentOrdersTable->setItem(0, 2, new QTableWidgetItem("¥199.00"));
    m_recentOrdersTable->setItem(0, 3, new QTableWidgetItem("已完成"));
    
    m_recentOrdersTable->setItem(1, 0, new QTableWidgetItem("ORD-002"));
    m_recentOrdersTable->setItem(1, 1, new QTableWidgetItem("李四"));
    m_recentOrdersTable->setItem(1, 2, new QTableWidgetItem("¥299.00"));
    m_recentOrdersTable->setItem(1, 3, new QTableWidgetItem("处理中"));
    
    m_recentOrdersTable->setItem(2, 0, new QTableWidgetItem("ORD-003"));
    m_recentOrdersTable->setItem(2, 1, new QTableWidgetItem("王五"));
    m_recentOrdersTable->setItem(2, 2, new QTableWidgetItem("¥499.00"));
    m_recentOrdersTable->setItem(2, 3, new QTableWidgetItem("已完成"));
    
    m_recentOrdersTable->setItem(3, 0, new QTableWidgetItem("ORD-004"));
    m_recentOrdersTable->setItem(3, 1, new QTableWidgetItem("赵六"));
    m_recentOrdersTable->setItem(3, 2, new QTableWidgetItem("¥99.00"));
    m_recentOrdersTable->setItem(3, 3, new QTableWidgetItem("待支付"));
    
    m_recentOrdersTable->setItem(4, 0, new QTableWidgetItem("ORD-005"));
    m_recentOrdersTable->setItem(4, 1, new QTableWidgetItem("钱七"));
    m_recentOrdersTable->setItem(4, 2, new QTableWidgetItem("¥399.00"));
    m_recentOrdersTable->setItem(4, 3, new QTableWidgetItem("处理中"));
    
    ordersLayout->addWidget(m_recentOrdersTable);
    m_mainLayout->addWidget(ordersGroup);
}

void MerchantDashboard::setupQuickActions() {
    QGroupBox *actionsGroup = new QGroupBox("快速操作");
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsGroup);
    
    QPushButton *addProductBtn = new QPushButton("添加产品");
    QPushButton *viewOrdersBtn = new QPushButton("查看订单");
    QPushButton *manageCustomersBtn = new QPushButton("管理客户");
    QPushButton *settingsBtn = new QPushButton("设置");
    
    actionsLayout->addWidget(addProductBtn);
    actionsLayout->addWidget(viewOrdersBtn);
    actionsLayout->addWidget(manageCustomersBtn);
    actionsLayout->addWidget(settingsBtn);
    
    m_mainLayout->addWidget(actionsGroup);
}

void MerchantDashboard::onRefreshClicked() {
    // 这里可以添加刷新数据的逻辑
    // 例如从服务器获取最新的销售数据和订单信息
}
