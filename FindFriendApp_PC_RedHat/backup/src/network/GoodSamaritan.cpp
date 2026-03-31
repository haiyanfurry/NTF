#include "GoodSamaritan.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

GoodSamaritan::GoodSamaritan() : m_server(nullptr) {}

GoodSamaritan::~GoodSamaritan() {
    stop();
}

GoodSamaritan& GoodSamaritan::instance() {
    static GoodSamaritan s;
    return s;
}

void GoodSamaritan::start(int port) {
    if (m_server) stop();
    
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &GoodSamaritan::onNewConnection);
    
    if (m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "🐺 老好人服务器已启动，端口:" << port;
    } else {
        qDebug() << "❌ 启动服务器失败:" << m_server->errorString();
    }
}

void GoodSamaritan::stop() {
    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
    }
    for (auto client : m_clients) {
        client->close();
        client->deleteLater();
    }
    m_clients.clear();
}

void GoodSamaritan::reportLocation(const User& user) {
    // TODO: 发送位置到服务器
}

void GoodSamaritan::requestNearbyUsers(double lat, double lon, double radius) {
    // TODO: 请求附近用户
    QList<User> emptyList;
    emit nearbyUsersUpdated(emptyList);
}

bool GoodSamaritan::sendFriendRequest(uint32_t targetId) {
    if (checkCooldown(targetId)) return false;
    m_cooldowns[targetId] = QDateTime::currentDateTime().addSecs(60);
    return true;
}

bool GoodSamaritan::checkCooldown(uint32_t targetId) {
    if (!m_cooldowns.contains(targetId)) return false;
    return m_cooldowns[targetId] > QDateTime::currentDateTime();
}

void GoodSamaritan::onNewConnection() {
    QTcpSocket* client = m_server->nextPendingConnection();
    m_clients.append(client);
    connect(client, &QTcpSocket::disconnected, this, &GoodSamaritan::onClientDisconnected);
    connect(client, &QTcpSocket::readyRead, this, &GoodSamaritan::onClientReadyRead);
}

void GoodSamaritan::onClientDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    m_clients.removeAll(client);
    client->deleteLater();
}

void GoodSamaritan::onClientReadyRead() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    // 处理接收到的数据
}
