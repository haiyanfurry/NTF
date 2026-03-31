#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>

class Settings {
public:
    static Settings& instance();
    
    bool goodSamaritanMode() const { return m_settings.value("goodSamaritanMode", false).toBool(); }
    void setGoodSamaritanMode(bool enabled) { m_settings.setValue("goodSamaritanMode", enabled); }
    
    QString nickname() const { return m_settings.value("nickname", "旅行者").toString(); }
    void setNickname(const QString& name) { m_settings.setValue("nickname", name); }
    
    QString signature() const { return m_settings.value("signature", "寻找志同道合的朋友~").toString(); }
    void setSignature(const QString& sig) { m_settings.setValue("signature", sig); }
    
    uint16_t tags() const { return m_settings.value("tags", 1).toUInt(); }
    void setTags(uint16_t tags) { m_settings.setValue("tags", tags); }
    
    bool animationEnabled() const { return m_settings.value("animationEnabled", true).toBool(); }
    void setAnimationEnabled(bool enabled) { m_settings.setValue("animationEnabled", enabled); }
    
private:
    Settings() : m_settings("FindFriend", "FindFriend") {}
    QSettings m_settings;
};
#endif
