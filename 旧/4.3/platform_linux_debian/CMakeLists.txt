# Debian Linux platform CMakeLists.txt

# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(FindFriendApp_DebianLinux)

# 包含公共核心
# 注意：公共核心和UI由根目录CMakeLists.txt统一构建

# 设置输出目录为平台根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Debian Linux平台特定代码
add_executable(FindFriendApp_Debian main.c)

target_link_libraries(FindFriendApp_Debian PRIVATE
    FindFriendCore
    FindFriendUI
    pthread
)

target_include_directories(FindFriendApp_Debian PRIVATE
    ../common_core
    ../common_ui
)

# Debian专用调试端 - 仅在Debian平台编译
if(EXISTS "/etc/debian_version")
    add_executable(FindFriendDebugger debugger_main.c)
    
    target_link_libraries(FindFriendDebugger PRIVATE
        FindFriendCore
        FindFriendUI
        pthread
    )
    
    target_include_directories(FindFriendDebugger PRIVATE
        ../common_core
        ../common_ui
    )
    
    message(STATUS "Debian-specific debugger enabled")
else()
    message(STATUS "Debian-specific debugger disabled on non-Debian platform")
endif()