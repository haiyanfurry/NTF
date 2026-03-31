#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

struct User {
    uint32_t id;
    QString nickname;
    QString emoji;
    QString signature;
    double latitude;
    double longitude;
    uint16_t tags;  // 1:furry 2:anime 4:cos
    bool online;
    QDateTime lastSeen;
    
    User() : id(0), latitude(0), longitude(0), tags(1), online(false) {}
};
#endif
