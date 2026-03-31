#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("商家登录");
    setModal(true);
    setFixedSize(350, 200);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *userLayout = new QHBoxLayout();
    QLabel *userLabel = new QLabel("用户名:", this);
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("输入商家用户名");
    userLayout->addWidget(userLabel);
    userLayout->addWidget(m_usernameEdit);
    
    QHBoxLayout *passLayout = new QHBoxLayout();
    QLabel *passLabel = new QLabel("密码:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("输入密码");
    passLayout->addWidget(passLabel);
    passLayout->addWidget(m_passwordEdit);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_cancelButton = new QPushButton("取消", this);
    m_loginButton = new QPushButton("登录", this);
    btnLayout->addStretch();
    btnLayout->addWidget(m_cancelButton);
    btnLayout->addWidget(m_loginButton);
    
    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(passLayout);
    mainLayout->addLayout(btnLayout);
    
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
}

LoginDialog::~LoginDialog() {
}

QString LoginDialog::username() const {
    return m_usernameEdit->text();
}

void LoginDialog::onLoginClicked() {
    if (m_usernameEdit->text().isEmpty() || m_passwordEdit->text().isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }
    accept();
}