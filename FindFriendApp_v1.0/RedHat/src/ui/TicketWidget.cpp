#include "TicketWidget.h"
#include <QVBoxLayout>
#include <QLabel>

TicketWidget::TicketWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("🎫 我的票务 (开发中)", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
