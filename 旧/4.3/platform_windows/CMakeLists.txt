# Windows platform CMakeLists.txt

# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(FindFriendApp_Windows)

# 设置输出目录为平台根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 包含公共核心
add_subdirectory(../common_core ${CMAKE_BINARY_DIR}/common_core)

# 包含公共UI
add_subdirectory(../common_ui ${CMAKE_BINARY_DIR}/common_ui)

# Windows平台特定代码
add_executable(FindFriendApp main.c)

target_link_libraries(FindFriendApp PRIVATE
    FindFriendCore
    FindFriendUI
    ws2_32
)

target_include_directories(FindFriendApp PRIVATE
    ../common_core
    ../common_ui
)