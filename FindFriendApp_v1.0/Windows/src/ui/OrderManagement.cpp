#include "OrderManagement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>

OrderManagement::OrderManagement(QWidget *parent) : QWidget(parent) {
    m_mainLayout = new QVBoxLayout(this);
    setupOrderManagement();
    setLayout(m_mainLayout);
}

OrderManagement::~OrderManagement() {
}

void OrderManagement::setupOrderManagement() {
    // 标题
    QLabel *titleLabel = new QLabel("订单管理");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    
    // 搜索栏
    setupSearchBar();
    
    // 订单表格
    setupOrderTable();
    
    // 操作按钮
    setupActionButtons();
}

void OrderManagement::setupSearchBar() {
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("搜索:");
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("输入订单号或客户名称");
    m_searchButton = new QPushButton("搜索");
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    
    m_mainLayout->addLayout(searchLayout);
    
    connect(m_searchButton, &QPushButton::clicked, this, &OrderManagement::onSearchClicked);
}

void OrderManagement::setupOrderTable() {
    m_orderTable = new QTableWidget();
    m_orderTable->setColumnCount(6);
    m_orderTable->setHorizontalHeaderLabels({"订单号", "客户", "产品", "金额", "状态", "下单时间"});
    m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 添加示例数据
    m_orderTable->setRowCount(5);
    m_orderTable->setItem(0, 0, new QTableWidgetItem("ORD-001"));
    m_orderTable->setItem(0, 1, new QTableWidgetItem("张三"));
    m_orderTable->setItem(0, 2, new QTableWidgetItem("COMICUP 30 普通票"));
    m_orderTable->setItem(0, 3, new QTableWidgetItem("¥80.00"));
    m_orderTable->setItem(0, 4, new QTableWidgetItem("已完成"));
    m_orderTable->setItem(0, 5, new QTableWidgetItem("2026-03-30 10:00"));
    
    m_orderTable->setItem(1, 0, new QTableWidgetItem("ORD-002"));
    m_orderTable->setItem(1, 1, new QTableWidgetItem("李四"));
    m_orderTable->setItem(1, 2, new QTableWidgetItem("COMICUP 30 VIP票"));
    m_orderTable->setItem(1, 3, new QTableWidgetItem("¥150.00"));
    m_orderTable->setItem(1, 4, new QTableWidgetItem("处理中"));
    m_orderTable->setItem(1, 5, new QTableWidgetItem("2026-03-30 11:30"));
    
    m_orderTable->setItem(2, 0, new QTableWidgetItem("ORD-003"));
    m_orderTable->setItem(2, 1, new QTableWidgetItem("王五"));
    m_orderTable->setItem(2, 2, new QTableWidgetItem("Furry 冬日祭 门票"));
    m_orderTable->setItem(2, 3, new QTableWidgetItem("¥100.00"));
    m_orderTable->setItem(2, 4, new QTableWidgetItem("已完成"));
    m_orderTable->setItem(2, 5, new QTableWidgetItem("2026-03-30 12:45"));
    
    m_orderTable->setItem(3, 0, new QTableWidgetItem("ORD-004"));
    m_orderTable->setItem(3, 1, new QTableWidgetItem("赵六"));
    m_orderTable->setItem(3, 2, new QTableWidgetItem("Bilibili World 门票"));
    m_orderTable->setItem(3, 3, new QTableWidgetItem("¥120.00"));
    m_orderTable->setItem(3, 4, new QTableWidgetItem("待支付"));
    m_orderTable->setItem(3, 5, new QTableWidgetItem("2026-03-30 14:20"));
    
    m_orderTable->setItem(4, 0, new QTableWidgetItem("ORD-005"));
    m_orderTable->setItem(4, 1, new QTableWidgetItem("钱七"));
    m_orderTable->setItem(4, 2, new QTableWidgetItem("同人创作展 门票"));
    m_orderTable->setItem(4, 3, new QTableWidgetItem("¥60.00"));
    m_orderTable->setItem(4, 4, new QTableWidgetItem("处理中"));
    m_orderTable->setItem(4, 5, new QTableWidgetItem("2026-03-30 15:50"));
    
    m_mainLayout->addWidget(m_orderTable);
}

void OrderManagement::setupActionButtons() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_processButton = new QPushButton("处理订单");
    m_cancelButton = new QPushButton("取消订单");
    m_refundButton = new QPushButton("退款");
    
    buttonLayout->addWidget(m_processButton);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_refundButton);
    
    m_mainLayout->addLayout(buttonLayout);
    
    connect(m_processButton, &QPushButton::clicked, this, &OrderManagement::onProcessOrderClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &OrderManagement::onCancelOrderClicked);
    connect(m_refundButton, &QPushButton::clicked, this, &OrderManagement::onRefundOrderClicked);
}

void OrderManagement::onProcessOrderClicked() {
    // 这里可以添加处理订单的逻辑
    QMessageBox::information(this, "提示", "处理订单功能开发中");
}

void OrderManagement::onCancelOrderClicked() {
    // 这里可以添加取消订单的逻辑
    QMessageBox::information(this, "提示", "取消订单功能开发中");
}

void OrderManagement::onRefundOrderClicked() {
    // 这里可以添加退款的逻辑
    QMessageBox::information(this, "提示", "退款功能开发中");
}

void OrderManagement::onSearchClicked() {
    // 这里可以添加搜索订单的逻辑
    QMessageBox::information(this, "提示", "搜索订单功能开发中");
}
