#include "HomePage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    auto mainLayout = new QVBoxLayout(this);
    auto navLayout = new QHBoxLayout();

    // 顶部导航
    navLayout->addWidget(new QPushButton("首页"));
    navLayout->addWidget(new QPushButton("聊天"));
    navLayout->addWidget(new QPushButton("我的"));

    // 占位提示
    auto label = new QLabel("首页框架已就绪\n可二次开发");
    label->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(navLayout);
    mainLayout->addWidget(label);
    mainLayout->setContentsMargins(20, 20, 20, 20);
}
