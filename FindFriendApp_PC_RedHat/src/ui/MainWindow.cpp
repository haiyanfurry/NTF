#include "MainWindow.h"
#include "LoginDialog.h"
#include "ExhibitionWidget.h"
#include "TicketWidget.h"
#include "TransactionWidget.h"
#include "GalleryWidget.h"
#include "MessageWidget.h"
#include "MapWidget.h"
#include "SettingsWidget.h"
#include "../settings/Settings.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QWebEngineView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_loggedIn(false) {
    setWindowTitle("FindFriend - 同城交友·展会助手");
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
    m_tabWidget->addTab(new MessageWidget(this), "💬 消息");
    m_tabWidget->addTab(new ExhibitionWidget(this), "🎪 展会");
    m_tabWidget->addTab(new TicketWidget(this), "🎫 票务");
    m_tabWidget->addTab(new TransactionWidget(this), "💰 交易");
    m_tabWidget->addTab(new GalleryWidget(this), "📸 相册");
    m_tabWidget->addTab(new MapWidget(this), "🗺️ 地图");
    m_tabWidget->addTab(new SettingsWidget(this), "⚙️ 设置");
    
    m_tabWidget->setEnabled(false);  // 登录后才启用
    layout->addWidget(m_tabWidget);
    central->setLayout(layout);
    setCentralWidget(central);
}

MainWindow::~MainWindow() {}

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
