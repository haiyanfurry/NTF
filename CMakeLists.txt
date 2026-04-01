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

# 包含平台目录
if(WIN32)
    # Windows平台
    message(STATUS "Windows platform detected")
    add_subdirectory(platform_windows)
elseif(APPLE)
    # iOS和Mac平台
    message(STATUS "iOS/Mac platform detected")
    add_subdirectory(platform_macos)
    add_subdirectory(platform_ios)
elseif(UNIX)
    # 检测Linux发行版
    message(STATUS "Linux platform detected")
    
    # 检测具体的Linux发行版
    if(EXISTS "/etc/debian_version")
        message(STATUS "Debian Linux detected")
        add_subdirectory(platform_linux_debian)
    elseif(EXISTS "/etc/redhat-release")
        message(STATUS "RedHat Linux detected")
        add_subdirectory(platform_linux_redhat)
    elseif(EXISTS "/etc/arch-release")
        message(STATUS "Arch Linux detected")
        add_subdirectory(platform_linux_arch)
    elseif(EXISTS "/etc/gentoo-release")
        message(STATUS "Gentoo Linux detected")
        # Gentoo只使用Makefile，不使用CMake
    elseif(EXISTS "/etc/lfs-release")
        message(STATUS "LFS Linux detected")
        # LFS只使用Makefile，不使用CMake
    else()
        message(STATUS "Unknown Linux distribution")
        add_subdirectory(platform_linux_debian)
    endif()
elseif(ANDROID)
    # Android平台
    message(STATUS "Android platform detected")
    add_subdirectory(platform_android)
else()
    message(STATUS "Unknown platform, using generic build")
endif()

# 版本信息
message(STATUS "FindFriendApp version: ${PROJECT_VERSION}")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")