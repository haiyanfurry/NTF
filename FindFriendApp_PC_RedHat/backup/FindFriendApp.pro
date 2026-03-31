QT += core widgets network webenginewidgets
CONFIG += c++17

TARGET = FindFriendApp
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/core/AppCore.cpp \
    src/ui/MainWindow.cpp \
    src/ui/FriendListWidget.cpp \
    src/ui/MapViewWidget.cpp \
    src/ui/ChatWidget.cpp \
    src/ui/SettingsDialog.cpp \
    src/ui/PersonalCenterDialog.cpp \
    src/models/User.cpp \
    src/network/GoodSamaritan.cpp \
    src/settings/Settings.cpp

HEADERS += \
    src/core/AppCore.h \
    src/ui/MainWindow.h \
    src/ui/FriendListWidget.h \
    src/ui/MapViewWidget.h \
    src/ui/ChatWidget.h \
    src/ui/SettingsDialog.h \
    src/ui/PersonalCenterDialog.h \
    src/models/User.h \
    src/network/GoodSamaritan.h \
    src/settings/Settings.h

RESOURCES += resources.qrc

target.path = /usr/local/bin
INSTALLS += target
