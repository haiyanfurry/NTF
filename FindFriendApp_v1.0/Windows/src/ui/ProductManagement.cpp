#include "ProductManagement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>

ProductManagement::ProductManagement(QWidget *parent) : QWidget(parent) {
    m_mainLayout = new QVBoxLayout(this);
    setupProductManagement();
    setLayout(m_mainLayout);
}

ProductManagement::~ProductManagement() {
}

void ProductManagement::setupProductManagement() {
    // 标题
    QLabel *titleLabel = new QLabel("产品管理");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    
    // 搜索栏
    setupSearchBar();
    
    // 产品表格
    setupProductTable();
    
    // 操作按钮
    setupActionButtons();
}

void ProductManagement::setupSearchBar() {
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("搜索:");
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("输入产品名称或ID");
    m_searchButton = new QPushButton("搜索");
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    
    m_mainLayout->addLayout(searchLayout);
    
    connect(m_searchButton, &QPushButton::clicked, this, &ProductManagement::onSearchClicked);
}

void ProductManagement::setupProductTable() {
    m_productTable = new QTableWidget();
    m_productTable->setColumnCount(5);
    m_productTable->setHorizontalHeaderLabels({"产品ID", "产品名称", "价格", "库存", "状态"});
    m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 添加示例数据
    m_productTable->setRowCount(5);
    m_productTable->setItem(0, 0, new QTableWidgetItem("P001"));
    m_productTable->setItem(0, 1, new QTableWidgetItem("COMICUP 30 普通票"));
    m_productTable->setItem(0, 2, new QTableWidgetItem("¥80.00"));
    m_productTable->setItem(0, 3, new QTableWidgetItem("100"));
    m_productTable->setItem(0, 4, new QTableWidgetItem("在售"));
    
    m_productTable->setItem(1, 0, new QTableWidgetItem("P002"));
    m_productTable->setItem(1, 1, new QTableWidgetItem("COMICUP 30 VIP票"));
    m_productTable->setItem(1, 2, new QTableWidgetItem("¥150.00"));
    m_productTable->setItem(1, 3, new QTableWidgetItem("50"));
    m_productTable->setItem(1, 4, new QTableWidgetItem("在售"));
    
    m_productTable->setItem(2, 0, new QTableWidgetItem("P003"));
    m_productTable->setItem(2, 1, new QTableWidgetItem("Furry 冬日祭 门票"));
    m_productTable->setItem(2, 2, new QTableWidgetItem("¥100.00"));
    m_productTable->setItem(2, 3, new QTableWidgetItem("80"));
    m_productTable->setItem(2, 4, new QTableWidgetItem("在售"));
    
    m_productTable->setItem(3, 0, new QTableWidgetItem("P004"));
    m_productTable->setItem(3, 1, new QTableWidgetItem("Bilibili World 门票"));
    m_productTable->setItem(3, 2, new QTableWidgetItem("¥120.00"));
    m_productTable->setItem(3, 3, new QTableWidgetItem("120"));
    m_productTable->setItem(3, 4, new QTableWidgetItem("在售"));
    
    m_productTable->setItem(4, 0, new QTableWidgetItem("P005"));
    m_productTable->setItem(4, 1, new QTableWidgetItem("同人创作展 门票"));
    m_productTable->setItem(4, 2, new QTableWidgetItem("¥60.00"));
    m_productTable->setItem(4, 3, new QTableWidgetItem("150"));
    m_productTable->setItem(4, 4, new QTableWidgetItem("在售"));
    
    m_mainLayout->addWidget(m_productTable);
}

void ProductManagement::setupActionButtons() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("添加产品");
    m_editButton = new QPushButton("编辑产品");
    m_deleteButton = new QPushButton("删除产品");
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    
    m_mainLayout->addLayout(buttonLayout);
    
    connect(m_addButton, &QPushButton::clicked, this, &ProductManagement::onAddProductClicked);
    connect(m_editButton, &QPushButton::clicked, this, &ProductManagement::onEditProductClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProductManagement::onDeleteProductClicked);
}

void ProductManagement::onAddProductClicked() {
    // 这里可以添加添加产品的逻辑
    QMessageBox::information(this, "提示", "添加产品功能开发中");
}

void ProductManagement::onEditProductClicked() {
    // 这里可以添加编辑产品的逻辑
    QMessageBox::information(this, "提示", "编辑产品功能开发中");
}

void ProductManagement::onDeleteProductClicked() {
    // 这里可以添加删除产品的逻辑
    QMessageBox::information(this, "提示", "删除产品功能开发中");
}

void ProductManagement::onSearchClicked() {
    // 这里可以添加搜索产品的逻辑
    QMessageBox::information(this, "提示", "搜索产品功能开发中");
}
