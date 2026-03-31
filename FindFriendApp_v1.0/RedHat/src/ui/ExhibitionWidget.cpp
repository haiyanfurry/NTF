#include "ExhibitionWidget.h"
#include <QVBoxLayout>
#include <QLabel>

ExhibitionWidget::ExhibitionWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("🎪 展会信息 (开发中)", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
