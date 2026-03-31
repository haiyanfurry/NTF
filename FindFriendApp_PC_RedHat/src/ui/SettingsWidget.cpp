#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent) {
    m_mainLayout = new QVBoxLayout(this);
    
    // 设置IP设置
    setupIPSettings();
    
    // 设置服务器列表
    setupServerList();
    
    // 设置同城用户发现
    setupLocalDiscovery();
    
    setLayout(m_mainLayout);
}

SettingsWidget::~SettingsWidget() {
}

void SettingsWidget::setupIPSettings() {
    m_ipGroupBox = new QGroupBox("服务器IP设置");
    QVBoxLayout *layout = new QVBoxLayout(m_ipGroupBox);
    
    // 当前IP显示
    QHBoxLayout *currentIPLayout = new QHBoxLayout();
    QLabel *label = new QLabel("当前服务器IP:");
    m_currentIPLabel = new QLabel("未设置");
    currentIPLayout->addWidget(label);
    currentIPLayout->addWidget(m_currentIPLabel);
    layout->addLayout(currentIPLayout);
    
    // IP输入
    QHBoxLayout *ipInputLayout = new QHBoxLayout();
    label = new QLabel("服务器IP:");
    m_ipEntry = new QLineEdit();
    m_ipEntry->setPlaceholderText("输入服务器IP地址");
    m_setIPBtn = new QPushButton("设置IP");
    
    ipInputLayout->addWidget(label);
    ipInputLayout->addWidget(m_ipEntry);
    ipInputLayout->addWidget(m_setIPBtn);
    
    layout->addLayout(ipInputLayout);
    m_mainLayout->addWidget(m_ipGroupBox);
    
    connect(m_setIPBtn, &QPushButton::clicked, this, &SettingsWidget::onSetIPClicked);
}

void SettingsWidget::setupServerList() {
    m_serverListGroupBox = new QGroupBox("服务器列表");
    QVBoxLayout *layout = new QVBoxLayout(m_serverListGroupBox);
    
    // 服务器列表
    m_serverListWidget = new QListWidget();
    layout->addWidget(m_serverListWidget);
    
    // 添加服务器
    QHBoxLayout *addServerLayout = new QHBoxLayout();
    QLabel *label = new QLabel("服务器名称:");
    m_serverNameEntry = new QLineEdit();
    m_serverNameEntry->setPlaceholderText("输入服务器名称");
    
    label = new QLabel("服务器IP:");
    m_serverIPEntry = new QLineEdit();
    m_serverIPEntry->setPlaceholderText("输入服务器IP地址");
    
    m_addServerBtn = new QPushButton("添加服务器");
    
    addServerLayout->addWidget(new QLabel("服务器名称:"));
    addServerLayout->addWidget(m_serverNameEntry);
    addServerLayout->addWidget(new QLabel("服务器IP:"));
    addServerLayout->addWidget(m_serverIPEntry);
    addServerLayout->addWidget(m_addServerBtn);
    
    layout->addLayout(addServerLayout);
    m_mainLayout->addWidget(m_serverListGroupBox);
    
    connect(m_addServerBtn, &QPushButton::clicked, this, &SettingsWidget::onAddServerClicked);
}

void SettingsWidget::setupLocalDiscovery() {
    m_discoveryGroupBox = new QGroupBox("同城用户发现");
    QVBoxLayout *layout = new QVBoxLayout(m_discoveryGroupBox);
    
    // 控制按钮
    QHBoxLayout *controlLayout = new QHBoxLayout();
    m_startDiscoveryBtn = new QPushButton("开始发现");
    m_stopDiscoveryBtn = new QPushButton("停止发现");
    
    controlLayout->addWidget(m_startDiscoveryBtn);
    controlLayout->addWidget(m_stopDiscoveryBtn);
    layout->addLayout(controlLayout);
    
    // 设备列表
    m_deviceListWidget = new QListWidget();
    layout->addWidget(m_deviceListWidget);
    
    m_mainLayout->addWidget(m_discoveryGroupBox);
    
    connect(m_startDiscoveryBtn, &QPushButton::clicked, this, &SettingsWidget::onStartDiscoveryClicked);
    connect(m_stopDiscoveryBtn, &QPushButton::clicked, this, &SettingsWidget::onStopDiscoveryClicked);
}

void SettingsWidget::onSetIPClicked() {
    QString ip = m_ipEntry->text();
    if (!ip.isEmpty()) {
        // 这里应该调用设置IP的函数
        m_currentIPLabel->setText(ip);
        QMessageBox::information(this, "成功", "服务器IP已设置");
    }
}

void SettingsWidget::onAddServerClicked() {
    QString name = m_serverNameEntry->text();
    QString ip = m_serverIPEntry->text();
    if (!name.isEmpty() && !ip.isEmpty()) {
        // 这里应该调用添加服务器的函数
        QString itemText = name + " (" + ip + ")";
        m_serverListWidget->addItem(itemText);
        m_serverNameEntry->clear();
        m_serverIPEntry->clear();
        QMessageBox::information(this, "成功", "服务器已添加");
    }
}

void SettingsWidget::onStartDiscoveryClicked() {
    // 这里应该调用开始发现的函数
    QMessageBox::information(this, "成功", "开始同城用户发现");
}

void SettingsWidget::onStopDiscoveryClicked() {
    // 这里应该调用停止发现的函数
    QMessageBox::information(this, "成功", "停止同城用户发现");
}