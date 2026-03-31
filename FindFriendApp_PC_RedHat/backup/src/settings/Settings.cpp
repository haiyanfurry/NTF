#include "Settings.h"

Settings::Settings() : m_settings("FindFriend", "FindFriend") {}

Settings& Settings::instance() {
    static Settings s;
    return s;
}

void Settings::setGoodSamaritanMode(bool enabled) {
    m_settings.setValue("goodSamaritanMode", enabled);
}

void Settings::setServerPort(int port) {
    m_settings.setValue("serverPort", port);
}

void Settings::setMapZoom(double zoom) {
    m_settings.setValue("mapZoom", zoom);
}

void Settings::setMapLat(double lat) {
    m_settings.setValue("mapLat", lat);
}

void Settings::setMapLng(double lng) {
    m_settings.setValue("mapLng", lng);
}

void Settings::setNickname(const QString& name) {
    m_settings.setValue("nickname", name);
}

void Settings::setSignature(const QString& sig) {
    m_settings.setValue("signature", sig);
}

void Settings::setTags(uint16_t tags) {
    m_settings.setValue("tags", tags);
}

void Settings::setAnimationEnabled(bool enabled) {
    m_settings.setValue("animationEnabled", enabled);
}
