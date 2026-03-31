QT += core widgets network webenginewidgets multimedia
CONFIG += c++17

TARGET = FindFriendApp
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/ui/MainWindow.cpp \
    src/ui/LoginDialog.cpp \
    src/ui/ExhibitionWidget.cpp \
    src/ui/TicketWidget.cpp \
    src/ui/TransactionWidget.cpp \
    src/ui/GalleryWidget.cpp \
    src/ui/MessageWidget.cpp \
    src/ui/MapWidget.cpp \
    src/models/User.cpp \
    src/network/NetworkManager.cpp \
    src/settings/Settings.cpp

HEADERS += \
    src/ui/MainWindow.h \
    src/ui/LoginDialog.h \
    src/ui/ExhibitionWidget.h \
    src/ui/TicketWidget.h \
    src/ui/TransactionWidget.h \
    src/ui/GalleryWidget.h \
    src/ui/MessageWidget.h \
    src/ui/MapWidget.h \
    src/models/User.h \
    src/network/NetworkManager.h \
    src/settings/Settings.h

RESOURCES += resources.qrc

target.path = /usr/local/bin
INSTALLS += target
