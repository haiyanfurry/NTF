#include "TransactionWidget.h"
#include <QVBoxLayout>
#include <QLabel>

TransactionWidget::TransactionWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("💰 交易市场 (开发中)", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
