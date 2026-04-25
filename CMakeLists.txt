cmake_minimum_required(VERSION 3.10)

# 项目名称和版本号
project(FindFriendApp VERSION 4.0)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 统一输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含公共核心目录
add_subdirectory(common_core)

# 包含公共UI目录
add_subdirectory(common_ui)

# 添加 CLI 调试控制台
add_executable(FindFriendCLI
    common_core/core_cli.c
)

target_link_libraries(FindFriendCLI
    FindFriendCore
)

target_include_directories(FindFriendCLI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/common_core
)

# 包含平台目录
if(WIN32)
    # Windows平台
    message(STATUS "Windows platform detected")
    add_subdirectory(platforms/windows)
elseif(APPLE)
    # iOS和Mac平台
    message(STATUS "iOS/Mac platform detected")
    add_subdirectory(platforms/macos)
    add_subdirectory(platforms/ios)
elseif(UNIX)
    # 检测Unix/BSD平台
    message(STATUS "Unix/BSD platform detected")

    if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        message(STATUS "FreeBSD detected")
        add_subdirectory(platforms/unix/freebsd)
    elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
        message(STATUS "OpenBSD detected")
        add_subdirectory(platforms/unix/openbsd)
    elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
        message(STATUS "NetBSD detected")
        add_subdirectory(platforms/unix/netbsd)
    elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly")
        message(STATUS "DragonFly BSD detected")
        # DragonFly 使用 FreeBSD 兼容代码
        add_subdirectory(platforms/unix/freebsd)
    elseif(EXISTS "/etc/debian_version")
        message(STATUS "Debian Linux detected")
        add_subdirectory(platforms/linux/debian)
    elseif(EXISTS "/etc/redhat-release")
        message(STATUS "RedHat Linux detected")
        add_subdirectory(platforms/linux/redhat)
    elseif(EXISTS "/etc/arch-release")
        message(STATUS "Arch Linux detected")
        add_subdirectory(platforms/linux/arch)
    elseif(EXISTS "/etc/gentoo-release")
        message(STATUS "Gentoo Linux detected")
        # Gentoo只使用Makefile，不使用CMake
    elseif(EXISTS "/etc/lfs-release")
        message(STATUS "LFS Linux detected")
        # LFS只使用Makefile，不使用CMake
    else()
        message(STATUS "Unknown Unix-like system, using generic build")
        # 可以添加通用的 Unix 构建配置
    endif()
elseif(ANDROID)
    # Android平台
    message(STATUS "Android platform detected")
    add_subdirectory(platforms/android)
else()
    message(STATUS "Unknown platform, using generic build")
endif()

# 版本信息
message(STATUS "FindFriendApp version: ${PROJECT_VERSION}")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
