#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <stdint.h>

struct User {
    uint32_t id;
    QString username;
    QString nickname;
    QString avatar;
    QString signature;
    int identity;      // 0:normal,1:cos,2:bianhe,3:official,4:exhibitor
    uint16_t tags;     // bit flags: 1 furry, 2 anime, 4 cos
    double latitude;
    double longitude;
    bool locationEnabled;
    QDateTime lastSeen;
    bool online;
    QString exhibitionName;
    QString exhibitionUrl;
    int followers;
    int following;
    QString emoji;

    User() : id(0), identity(0), tags(1), latitude(0), longitude(0),
             locationEnabled(true), online(false), followers(0), following(0) {
        emoji = "🐾";
    }
};
#endif
