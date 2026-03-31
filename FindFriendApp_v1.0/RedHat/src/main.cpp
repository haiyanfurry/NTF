#include <QApplication>
#include <QFile>
#include <QIODevice>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 加载样式表
    QFile styleFile(":/style.qss");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString style = styleFile.readAll();
        app.setStyleSheet(style);
        styleFile.close();
    }
    
    MainWindow w;
    w.show();
    
    return app.exec();
}
