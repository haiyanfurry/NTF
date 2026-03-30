#include <QApplication>
#include <QMainWindow>
#include "AppCore.h"
#include "../Pages/HomePage.h"

int AppCore::run(int argc, char **argv) {
    QApplication a(argc, argv);

    QMainWindow w;
    w.setWindowTitle("福瑞扩列计划 - RedHat");
    w.setFixedSize(420, 600);
    w.setCentralWidget(new HomePage);
    w.show();

    return a.exec();
}
