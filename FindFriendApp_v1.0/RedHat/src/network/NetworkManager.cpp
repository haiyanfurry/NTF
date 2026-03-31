#include "NetworkManager.h"
#include <QTcpSocket>
#include <QDebug>

NetworkManager::NetworkManager() : m_server(nullptr) {}

NetworkManager& NetworkManager::instance() {
    static NetworkManager nm;
    return nm;
}

void NetworkManager::startServer(int port) {
    if (m_server) stopServer();
    m_server = new QTcpServer(this);
    if (m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "🐺 老好人服务器启动，端口:" << port;
    }
}

void NetworkManager::stopServer() {
    if (m_server) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
}

bool NetworkManager::isServerRunning() const {
    return m_server && m_server->isListening();
}

bool NetworkManager::sendFriendRequest(uint32_t targetId) {
    if (checkCooldown(targetId)) return false;
    m_cooldowns[targetId] = QDateTime::currentDateTime().addSecs(60);
    return true;
}

bool NetworkManager::checkCooldown(uint32_t targetId) {
    if (!m_cooldowns.contains(targetId)) return false;
    return m_cooldowns[targetId] > QDateTime::currentDateTime();
}
