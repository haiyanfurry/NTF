#include "MapWidget.h"
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QFile>

MapWidget::MapWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QWebEngineView *webView = new QWebEngineView(this);
    
    QFile htmlFile(":/map.html");
    if (htmlFile.open(QIODevice::ReadOnly)) {
        QString html = htmlFile.readAll();
        webView->setHtml(html);
        htmlFile.close();
    } else {
        webView->setHtml("<h3>地图加载失败</h3>");
    }
    layout->addWidget(webView);
}
