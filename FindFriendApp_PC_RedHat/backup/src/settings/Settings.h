#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings {
public:
    static Settings& instance();
    
    bool goodSamaritanMode() const { return m_settings.value("goodSamaritanMode", false).toBool(); }
    void setGoodSamaritanMode(bool enabled);
    
    int serverPort() const { return m_settings.value("serverPort", 8888).toInt(); }
    void setServerPort(int port);
    
    double mapZoom() const { return m_settings.value("mapZoom", 10.0).toDouble(); }
    void setMapZoom(double zoom);
    
    double mapLat() const { return m_settings.value("mapLat", 39.9042).toDouble(); }
    void setMapLat(double lat);
    
    double mapLng() const { return m_settings.value("mapLng", 116.4074).toDouble(); }
    void setMapLng(double lng);
    
    QString nickname() const { return m_settings.value("nickname", "旅行者").toString(); }
    void setNickname(const QString& name);
    
    QString signature() const { return m_settings.value("signature", "寻找志同道合的朋友~").toString(); }
    void setSignature(const QString& sig);
    
    uint16_t tags() const { return m_settings.value("tags", 1).toUInt(); }
    void setTags(uint16_t tags);
    
    bool animationEnabled() const { return m_settings.value("animationEnabled", true).toBool(); }
    void setAnimationEnabled(bool enabled);
    
private:
    Settings();
    QSettings m_settings;
};

#endif
