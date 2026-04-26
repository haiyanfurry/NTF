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

# 可选：启用 Rust 模块（需要 Cargo 安装）
option(FF_USE_RUST_MODULES "Enable Rust modules for security" ON)

# 可选：构建 UI 模块
option(FF_BUILD_UI "Build UI modules" OFF)

# ====================================================================
# 查找 Rust 和 Cargo
# ====================================================================
if(FF_USE_RUST_MODULES)
    # 检查静态库是否存在
    if(EXISTS "${CMAKE_SOURCE_DIR}/rust_modules/target/release/librust_security.a")
        set(FF_RUST_AVAILABLE ON)
        message(STATUS "Rust static libraries found")
    else()
        message(WARNING "Rust static libraries not found. Rust modules will NOT be linked.")
        set(FF_RUST_AVAILABLE OFF)
        set(FF_USE_RUST_MODULES OFF)
    endif()
endif()

# ====================================================================
# Rust 模块编译（使用 Cargo）
# ====================================================================
if(FF_USE_RUST_MODULES)
    message(STATUS "Rust modules enabled. Using pre-built static libs.")
    
    set(RUST_MODULE_DIR "${CMAKE_SOURCE_DIR}/rust_modules")
    set(RUST_BUILD_DIR "${CMAKE_SOURCE_DIR}/rust_modules/target/release")
    
    # 生成的静态库名称
    if(WIN32)
        set(RUST_NET_LIB "${RUST_BUILD_DIR}/rust_network.lib")
        set(RUST_CRYPTO_LIB "${RUST_BUILD_DIR}/rust_crypto.lib")
        set(RUST_SECURITY_LIB "${RUST_BUILD_DIR}/rust_security.lib")
    else()
        set(RUST_NET_LIB "${RUST_BUILD_DIR}/librust_network.a")
        set(RUST_CRYPTO_LIB "${RUST_BUILD_DIR}/librust_crypto.a")
        set(RUST_SECURITY_LIB "${RUST_BUILD_DIR}/librust_security.a")
    endif()
    
    # 创建接口库以链接 Rust 模块
    add_library(RustModules INTERFACE)
    
    # 包含 Rust 绑定头文件
    target_include_directories(RustModules INTERFACE "${CMAKE_SOURCE_DIR}/rust_modules/c_bindings")
    
    # 链接 Rust 静态库
    target_link_libraries(RustModules INTERFACE
        ${RUST_NET_LIB}
        ${RUST_CRYPTO_LIB}
        ${RUST_SECURITY_LIB}
    )
    
    # 添加操作系统特定的依赖库
    if(WIN32)
        target_link_libraries(RustModules INTERFACE ws2_32 userenv bcrypt advapi32)
    elseif(APPLE)
        target_link_libraries(RustModules INTERFACE resolv)
    else()
        # Linux / BSD / Other Unix
        target_link_libraries(RustModules INTERFACE pthread dl m)
    endif()
    
    message(STATUS "Rust modules enabled. Using static libs from: ${RUST_BUILD_DIR}")
else()
    message(STATUS "Rust modules disabled. Building with C-only components.")
endif()

# ====================================================================
# 公共核心模块
# ====================================================================
set(COMMON_CORE_SOURCES
    common_core/find_friend_core.c
    common_core/core_interface.c
    common_core/ff_safe_utils.c
)

if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/auth")
    file(GLOB AUTH_SOURCES "common_core/auth/*.c")
    list(APPEND COMMON_CORE_SOURCES ${AUTH_SOURCES})
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/data")
    file(GLOB DATA_SOURCES "common_core/data/*.c")
    list(APPEND COMMON_CORE_SOURCES ${DATA_SOURCES})
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/util")
    file(GLOB UTIL_SOURCES "common_core/util/*.c")
    list(APPEND COMMON_CORE_SOURCES ${UTIL_SOURCES})
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/business")
    file(GLOB BUSINESS_SOURCES "common_core/business/*.c")
    list(APPEND COMMON_CORE_SOURCES ${BUSINESS_SOURCES})
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/network")
    file(GLOB NETWORK_SOURCES "common_core/network/*.c")
    list(APPEND COMMON_CORE_SOURCES ${NETWORK_SOURCES})
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/security")
    file(GLOB SECURITY_SOURCES "common_core/security/*.c")
    list(APPEND COMMON_CORE_SOURCES ${SECURITY_SOURCES})
    
    # 添加 boot_check 目录
    if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/security/boot_check")
        file(GLOB BOOT_CHECK_SOURCES "common_core/security/boot_check/*.c")
        list(APPEND COMMON_CORE_SOURCES ${BOOT_CHECK_SOURCES})
    endif()
    
    # 添加 mitm_protect 目录
    if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/tls/mitm_protect")
        file(GLOB MITM_PROTECT_SOURCES "common_core/tls/mitm_protect/*.c")
        list(APPEND COMMON_CORE_SOURCES ${MITM_PROTECT_SOURCES})
    endif()
endif()

# 添加 bridge 目录
if(EXISTS "${CMAKE_SOURCE_DIR}/common_core/bridge")
    file(GLOB BRIDGE_SOURCES "common_core/bridge/*.c")
    list(APPEND COMMON_CORE_SOURCES ${BRIDGE_SOURCES})
endif()

add_library(FindFriendCore STATIC ${COMMON_CORE_SOURCES})

target_include_directories(FindFriendCore PUBLIC
    ${CMAKE_SOURCE_DIR}/common_core
    ${CMAKE_SOURCE_DIR}/common_ui
    ${CMAKE_SOURCE_DIR}/common_core/security/boot_check
    ${CMAKE_SOURCE_DIR}/common_core/tls/mitm_protect
)

if(FF_USE_RUST_MODULES)
    target_link_libraries(FindFriendCore PUBLIC RustModules)
endif()

# 链接 OpenSSL 库
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    target_link_libraries(FindFriendCore PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(FindFriendCore PUBLIC ${OPENSSL_INCLUDE_DIR})
endif()

# ====================================================================
# 公共 UI 模块
# ====================================================================
if(FF_BUILD_UI AND EXISTS "${CMAKE_SOURCE_DIR}/common_ui")
    file(GLOB_RECURSE COMMON_UI_SOURCES "common_ui/*.c")
    if(COMMON_UI_SOURCES)
        add_library(FindFriendUI STATIC ${COMMON_UI_SOURCES})
        target_include_directories(FindFriendUI PUBLIC ${CMAKE_SOURCE_DIR}/common_ui)
        target_link_libraries(FindFriendUI PUBLIC FindFriendCore)
    endif()
endif()

# ====================================================================
# CLI 调试工具
# ====================================================================
add_executable(FindFriendCLI
    common_core/core_cli.c
)

target_link_libraries(FindFriendCLI
    FindFriendCore
)

target_include_directories(FindFriendCLI PRIVATE
    ${CMAKE_SOURCE_DIR}/common_core
)

# ====================================================================
# 平台特定代码
# ====================================================================

# 包含平台目录
if(WIN32)
    if(EXISTS "${CMAKE_SOURCE_DIR}/platforms/windows")
        message(STATUS "Windows platform detected")
        add_subdirectory(platforms/windows)
    endif()
elseif(APPLE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/platforms/macos")
        message(STATUS "macOS platform detected")
        add_subdirectory(platforms/macos)
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/platforms/ios")
        message(STATUS "iOS platform detected")
        add_subdirectory(platforms/ios)
    endif()
elseif(UNIX)
    # 检测 Unix/BSD/Linux
    message(STATUS "Unix/Linux platform detected")

    if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/unix/freebsd")
        message(STATUS "FreeBSD detected")
        add_subdirectory(platforms/unix/freebsd)
    elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/unix/openbsd")
        message(STATUS "OpenBSD detected")
        add_subdirectory(platforms/unix/openbsd)
    elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/unix/netbsd")
        message(STATUS "NetBSD detected")
        add_subdirectory(platforms/unix/netbsd)
    elseif(EXISTS "/etc/debian_version" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/linux/debian")
        message(STATUS "Debian Linux detected")
        add_subdirectory(platforms/linux/debian)
    elseif(EXISTS "/etc/redhat-release" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/linux/redhat")
        message(STATUS "RedHat Linux detected")
        add_subdirectory(platforms/linux/redhat)
    elseif(EXISTS "/etc/arch-release" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/linux/arch")
        message(STATUS "Arch Linux detected")
        add_subdirectory(platforms/linux/arch)
    elseif(EXISTS "/etc/gentoo-release" AND EXISTS "${CMAKE_SOURCE_DIR}/platforms/linux/gentoo")
        message(STATUS "Gentoo Linux detected")
    else()
        message(STATUS "Unknown Linux distro, using default")
        if(EXISTS "${CMAKE_SOURCE_DIR}/platforms/linux/debian")
            add_subdirectory(platforms/linux/debian)
        elseif(EXISTS "${CMAKE_SOURCE_DIR}/platforms/linux/redhat")
            add_subdirectory(platforms/linux/redhat)
        endif()
    endif()
elseif(ANDROID)
    if(EXISTS "${CMAKE_SOURCE_DIR}/platforms/android")
        message(STATUS "Android platform detected")
        add_subdirectory(platforms/android)
    endif()
else()
    message(STATUS "Unknown platform, using generic build")
endif()

# ====================================================================
# 构建信息打印
# ====================================================================
message(STATUS "")
message(STATUS "=============================================")
message(STATUS "  FindFriend Build Configuration")
message(STATUS "=============================================")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  CMake: ${CMAKE_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Rust modules: ${FF_USE_RUST_MODULES}")
if(FF_USE_RUST_MODULES)
    message(STATUS "  Rust target: ${RUST_BUILD_TYPE}")
endif()
message(STATUS "=============================================")
message(STATUS "")
