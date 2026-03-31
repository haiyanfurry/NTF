cmake_minimum_required(VERSION 3.18.1)

project("findfriendapp")

add_library(
        findfriendapp
        SHARED
        native-lib.cpp
        network/GoodSamaritan.cpp
        network/MediaTransfer.cpp
        models/User.cpp
        models/Database.cpp
)

find_library(
        log-lib
        log
)

find_library(
        android-lib
        android
)

# 包含头文件目录
target_include_directories(
        findfriendapp
        PRIVATE
        network
        models
)

target_link_libraries(
        findfriendapp
        ${log-lib}
        ${android-lib}
)