#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QList>
#include "../models/User.h"

class NetworkManager : public QObject {
    Q_OBJECT
public:
    static NetworkManager& instance();
    
    void startServer(int port);
    void stopServer();
    bool isServerRunning() const;
    
    bool sendFriendRequest(uint32_t targetId);
    bool checkCooldown(uint32_t targetId);
    
signals:
    void friendRequestReceived(uint32_t fromId, const QString& name, const QString& signature);
    
private:
    NetworkManager();
    QTcpServer* m_server;
    QMap<uint32_t, QDateTime> m_cooldowns;
};
#endif
