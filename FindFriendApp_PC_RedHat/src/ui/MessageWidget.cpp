#include "MessageWidget.h"
#include <QVBoxLayout>
#include <QLabel>

MessageWidget::MessageWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("📬 消息列表 (开发中)", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
