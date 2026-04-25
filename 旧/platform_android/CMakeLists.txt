# Android platform CMakeLists.txt

# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(FindFriendApp_Android)

# 包含公共核心
add_subdirectory(../common_core ${CMAKE_BINARY_DIR}/common_core)

# 包含公共UI
add_subdirectory(../common_ui ${CMAKE_BINARY_DIR}/common_ui)

# Android平台特定代码
# 注意：Android平台主要使用Gradle构建，这里只作为占位