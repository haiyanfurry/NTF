#ifndef GOOD_SAMARITAN_H
#define GOOD_SAMARITAN_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include "../models/User.h"

class GoodSamaritan : public QObject {
    Q_OBJECT
public:
    static GoodSamaritan& instance();
    
    void start(int port);
    void stop();
    bool isRunning() const { return m_server && m_server->isListening(); }
    
    void reportLocation(const User& user);
    void requestNearbyUsers(double lat, double lon, double radius);
    bool sendFriendRequest(uint32_t targetId);
    bool checkCooldown(uint32_t targetId);
    
signals:
    void nearbyUsersUpdated(const QList<User>& users);
    void friendRequestReceived(uint32_t fromId, const QString& name, const QString& signature);
    
private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onClientReadyRead();
    
private:
    GoodSamaritan();
    ~GoodSamaritan();
    
    QTcpServer* m_server;
    QList<QTcpSocket*> m_clients;
    QMap<uint32_t, QDateTime> m_cooldowns;
};

#endif
