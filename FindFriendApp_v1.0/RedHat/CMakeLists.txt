cmake_minimum_required(VERSION 3.10)
project(FindFriendApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Network WebEngineWidgets Multimedia REQUIRED)

set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/LoginDialog.cpp
    src/ui/ExhibitionWidget.cpp
    src/ui/TicketWidget.cpp
    src/ui/TransactionWidget.cpp
    src/ui/GalleryWidget.cpp
    src/ui/MessageWidget.cpp
    src/ui/MapWidget.cpp
    src/ui/SettingsWidget.cpp
    src/models/User.cpp
    src/network/NetworkManager.cpp
    src/settings/Settings.cpp
)

set(HEADERS
    src/ui/MainWindow.h
    src/ui/LoginDialog.h
    src/ui/ExhibitionWidget.h
    src/ui/TicketWidget.h
    src/ui/TransactionWidget.h
    src/ui/GalleryWidget.h
    src/ui/MessageWidget.h
    src/ui/MapWidget.h
    src/ui/SettingsWidget.h
    src/models/User.h
    src/network/NetworkManager.h
    src/settings/Settings.h
)

set(RESOURCES resources.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::WebEngineWidgets
    Qt5::Multimedia
)
