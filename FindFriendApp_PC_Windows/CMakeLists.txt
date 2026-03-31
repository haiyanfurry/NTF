cmake_minimum_required(VERSION 3.10)
project(FindFriendAppMerchant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Network WebEngineWidgets REQUIRED)

set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/LoginDialog.cpp
    src/ui/MerchantDashboard.cpp
    src/ui/ProductManagement.cpp
    src/ui/OrderManagement.cpp
    src/ui/CustomerManagement.cpp
    src/ui/SettingsWidget.cpp
    src/models/User.cpp
    src/models/Product.cpp
    src/models/Order.cpp
    src/network/NetworkManager.cpp
    src/settings/Settings.cpp
)

set(HEADERS
    src/ui/MainWindow.h
    src/ui/LoginDialog.h
    src/ui/MerchantDashboard.h
    src/ui/ProductManagement.h
    src/ui/OrderManagement.h
    src/ui/CustomerManagement.h
    src/ui/SettingsWidget.h
    src/models/User.h
    src/models/Product.h
    src/models/Order.h
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
)

# 为Windows平台添加特定的编译选项
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "FindFriendAppMerchant"
    )
endif()