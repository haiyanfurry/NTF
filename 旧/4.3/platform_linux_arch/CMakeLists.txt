# Arch Linux platform CMakeLists.txt

# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(FindFriendApp_ArchLinux)

# 设置输出目录为平台根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 包含公共核心
# 注意：公共核心和UI由根目录CMakeLists.txt统一构建

# Arch Linux平台特定代码
add_executable(FindFriendApp_Arch main.c)

target_link_libraries(FindFriendApp_Arch PRIVATE
    FindFriendCore
    FindFriendUI
    pthread
)

target_include_directories(FindFriendApp_Arch PRIVATE
    ../common_core
    ../common_ui
)