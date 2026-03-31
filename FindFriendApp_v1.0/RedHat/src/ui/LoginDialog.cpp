#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("登录");
    setModal(true);
    setFixedSize(350, 200);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *userLayout = new QHBoxLayout();
    QLabel *userLabel = new QLabel("用户名:", this);
    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText("输入用户名");
    userLayout->addWidget(userLabel);
    userLayout->addWidget(m_userEdit);
    
    QHBoxLayout *passLayout = new QHBoxLayout();
    QLabel *passLabel = new QLabel("密码:", this);
    m_passEdit = new QLineEdit(this);
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setPlaceholderText("输入密码");
    passLayout->addWidget(passLabel);
    passLayout->addWidget(m_passEdit);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *cancelBtn = new QPushButton("取消", this);
    QPushButton *loginBtn = new QPushButton("登录", this);
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(loginBtn);
    
    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(passLayout);
    mainLayout->addLayout(btnLayout);
    
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
}

void LoginDialog::onLogin() {
    if (m_userEdit->text().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名");
        return;
    }
    accept();
}
