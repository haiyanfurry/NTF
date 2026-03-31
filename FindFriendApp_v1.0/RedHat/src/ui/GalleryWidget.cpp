#include "GalleryWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

GalleryWidget::GalleryWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("📸 相册 (支持批量选择最多250张)", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    
    QPushButton *selectBtn = new QPushButton("选择照片", this);
    connect(selectBtn, &QPushButton::clicked, [this](){
        QFileDialog::getOpenFileNames(this, "选择照片", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    });
    layout->addWidget(selectBtn);
}
