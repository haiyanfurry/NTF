#include "MainWindow.h"
#include "LoginDialog.h"
#include "MerchantDashboard.h"
#include "ProductManagement.h"
#include "OrderManagement.h"
#include "CustomerManagement.h"
#include "SettingsWidget.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_loggedIn(false) {
    setWindowTitle("FindFriend - 商家管理系统");
    resize(1400, 800);
    
    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *userMenu = menuBar->addMenu("👤 未登录");
    QAction *loginAction = userMenu->addAction("登录");
    connect(loginAction, &QAction::triggered, this, &MainWindow::showLoginDialog);
    setMenuBar(menuBar);
    
    // 创建主布局
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    m_tabWidget = new QTabWidget(this);
    
    // 添加各功能页
    m_tabWidget->addTab(new MerchantDashboard(this), "📊 仪表盘");
    m_tabWidget->addTab(new ProductManagement(this), "🛍️ 产品管理");
    m_tabWidget->addTab(new OrderManagement(this), "📋 订单管理");
    m_tabWidget->addTab(new CustomerManagement(this), "👥 客户管理");
    m_tabWidget->addTab(new SettingsWidget(this), "⚙️ 设置");
    
    m_tabWidget->setEnabled(false);  // 登录后才启用
    layout->addWidget(m_tabWidget);
    central->setLayout(layout);
    setCentralWidget(central);
}

MainWindow::~MainWindow() {
}

void MainWindow::showLoginDialog() {
    LoginDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        onLoginSuccess(dlg.username());
    }
}

void MainWindow::onLoginSuccess(const QString& username) {
    m_loggedIn = true;
    m_tabWidget->setEnabled(true);
    // 更新菜单栏
    QMenuBar *mb = menuBar();
    QAction *userAction = mb->actions().first();
    userAction->setText("👤 " + username);
}