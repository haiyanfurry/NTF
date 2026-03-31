#include "CustomerManagement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>

CustomerManagement::CustomerManagement(QWidget *parent) : QWidget(parent) {
    m_mainLayout = new QVBoxLayout(this);
    setupCustomerManagement();
    setLayout(m_mainLayout);
}

CustomerManagement::~CustomerManagement() {
}

void CustomerManagement::setupCustomerManagement() {
    // 标题
    QLabel *titleLabel = new QLabel("客户管理");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    
    // 搜索栏
    setupSearchBar();
    
    // 客户表格
    setupCustomerTable();
    
    // 操作按钮
    setupActionButtons();
}

void CustomerManagement::setupSearchBar() {
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("搜索:");
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("输入客户名称或ID");
    m_searchButton = new QPushButton("搜索");
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    
    m_mainLayout->addLayout(searchLayout);
    
    connect(m_searchButton, &QPushButton::clicked, this, &CustomerManagement::onSearchClicked);
}

void CustomerManagement::setupCustomerTable() {
    m_customerTable = new QTableWidget();
    m_customerTable->setColumnCount(5);
    m_customerTable->setHorizontalHeaderLabels({"客户ID", "客户名称", "联系方式", "注册时间", "订单数"});
    m_customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 添加示例数据
    m_customerTable->setRowCount(5);
    m_customerTable->setItem(0, 0, new QTableWidgetItem("C001"));
    m_customerTable->setItem(0, 1, new QTableWidgetItem("张三"));
    m_customerTable->setItem(0, 2, new QTableWidgetItem("13800138001"));
    m_customerTable->setItem(0, 3, new QTableWidgetItem("2026-03-01"));
    m_customerTable->setItem(0, 4, new QTableWidgetItem("5"));
    
    m_customerTable->setItem(1, 0, new QTableWidgetItem("C002"));
    m_customerTable->setItem(1, 1, new QTableWidgetItem("李四"));
    m_customerTable->setItem(1, 2, new QTableWidgetItem("13900139002"));
    m_customerTable->setItem(1, 3, new QTableWidgetItem("2026-03-05"));
    m_customerTable->setItem(1, 4, new QTableWidgetItem("3"));
    
    m_customerTable->setItem(2, 0, new QTableWidgetItem("C003"));
    m_customerTable->setItem(2, 1, new QTableWidgetItem("王五"));
    m_customerTable->setItem(2, 2, new QTableWidgetItem("13700137003"));
    m_customerTable->setItem(2, 3, new QTableWidgetItem("2026-03-10"));
    m_customerTable->setItem(2, 4, new QTableWidgetItem("7"));
    
    m_customerTable->setItem(3, 0, new QTableWidgetItem("C004"));
    m_customerTable->setItem(3, 1, new QTableWidgetItem("赵六"));
    m_customerTable->setItem(3, 2, new QTableWidgetItem("13600136004"));
    m_customerTable->setItem(3, 3, new QTableWidgetItem("2026-03-15"));
    m_customerTable->setItem(3, 4, new QTableWidgetItem("2"));
    
    m_customerTable->setItem(4, 0, new QTableWidgetItem("C005"));
    m_customerTable->setItem(4, 1, new QTableWidgetItem("钱七"));
    m_customerTable->setItem(4, 2, new QTableWidgetItem("13500135005"));
    m_customerTable->setItem(4, 3, new QTableWidgetItem("2026-03-20"));
    m_customerTable->setItem(4, 4, new QTableWidgetItem("4"));
    
    m_mainLayout->addWidget(m_customerTable);
}

void CustomerManagement::setupActionButtons() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("添加客户");
    m_editButton = new QPushButton("编辑客户");
    m_deleteButton = new QPushButton("删除客户");
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    
    m_mainLayout->addLayout(buttonLayout);
    
    connect(m_addButton, &QPushButton::clicked, this, &CustomerManagement::onAddCustomerClicked);
    connect(m_editButton, &QPushButton::clicked, this, &CustomerManagement::onEditCustomerClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &CustomerManagement::onDeleteCustomerClicked);
}

void CustomerManagement::onAddCustomerClicked() {
    // 这里可以添加添加客户的逻辑
    QMessageBox::information(this, "提示", "添加客户功能开发中");
}

void CustomerManagement::onEditCustomerClicked() {
    // 这里可以添加编辑客户的逻辑
    QMessageBox::information(this, "提示", "编辑客户功能开发中");
}

void CustomerManagement::onDeleteCustomerClicked() {
    // 这里可以添加删除客户的逻辑
    QMessageBox::information(this, "提示", "删除客户功能开发中");
}

void CustomerManagement::onSearchClicked() {
    // 这里可以添加搜索客户的逻辑
    QMessageBox::information(this, "提示", "搜索客户功能开发中");
}
