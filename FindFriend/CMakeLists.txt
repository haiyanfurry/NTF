# FindFriend 项目根 CMakeLists.txt
# 统一构建系统

cmake_minimum_required(VERSION 3.20)

project(FindFriend VERSION 5.0)

# ====================================================================
# 项目配置
# ====================================================================
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FF_BUILD_TESTS "Build tests" ON)
option(FF_USE_RUST "Enable Rust security modules" OFF)
option(FF_BUILD_CLI "Build CLI tools" ON)
option(FF_ENABLE_LOGGING "Enable logging" ON)

# ====================================================================
# 目录结构
# ====================================================================
set(FF_CORE_DIR "${CMAKE_SOURCE_DIR}/core")
set(FF_SECURITY_DIR "${CMAKE_SOURCE_DIR}/security_rust")
set(FF_PLATFORM_DIR "${CMAKE_SOURCE_DIR}/platform")
set(FF_UI_DIR "${CMAKE_SOURCE_DIR}/ui")
set(FF_TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools")
set(FF_INTERFACE_DIR "${FF_CORE_DIR}/interface")

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ====================================================================
# 查找 Rust/Cargo
# ====================================================================
if(FF_USE_RUST)
    find_program(CARGO_EXECUTABLE cargo REQUIRED)
    message(STATUS "Cargo found: ${CARGO_EXECUTABLE}")
    set(FF_RUST_BUILD_DIR "${CMAKE_BINARY_DIR}/rust_build")
endif()

# ====================================================================
# 编译选项
# ====================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FF_OPTIMIZATION_FLAGS "-O0 -g")
else()
    set(FF_OPTIMIZATION_FLAGS "-O3")
endif()

add_compile_options(
    ${FF_OPTIMIZATION_FLAGS}
    -Wall
    -Wextra
    -Wpedantic
    -Werror=implicit-function-declaration
)

# ====================================================================
# 构建核心模块
# ====================================================================
add_subdirectory(${FF_CORE_DIR} core)

# ====================================================================
# 构建安全模块 (Rust)
# ====================================================================
if(FF_USE_RUST)
    add_subdirectory(${FF_SECURITY_DIR} security_rust)
endif()

# ====================================================================
# 构建平台模块 (暂时禁用，使用 CLI 工具)
# ====================================================================
# if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#     if(EXISTS "${FF_PLATFORM_DIR}/linux")
#         add_subdirectory(${FF_PLATFORM_DIR}/linux platform_linux)
#     endif()
# elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#     if(EXISTS "${FF_PLATFORM_DIR}/windows")
#         add_subdirectory(${FF_PLATFORM_DIR}/windows platform_windows)
#     endif()
# elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#     if(EXISTS "${FF_PLATFORM_DIR}/macos")
#         add_subdirectory(${FF_PLATFORM_DIR}/macos platform_macos)
#     endif()
#     if(EXISTS "${FF_PLATFORM_DIR}/ios")
#         add_subdirectory(${FF_PLATFORM_DIR}/ios platform_ios)
#     endif()
# elseif(CMAKE_SYSTEM_NAME MATCHES "BSD")
#     if(EXISTS "${FF_PLATFORM_DIR}/bsd")
#         add_subdirectory(${FF_PLATFORM_DIR}/bsd platform_bsd)
#     endif()
# endif()

# ====================================================================
# 构建 UI
# ====================================================================
if(EXISTS "${FF_UI_DIR}")
    add_subdirectory(${FF_UI_DIR} ui)
endif()

# ====================================================================
# 构建工具
# ====================================================================
if(FF_BUILD_CLI)
    add_subdirectory(${FF_TOOLS_DIR}/cli cli)
endif()

# ====================================================================
# 安装目标
# ====================================================================
install(DIRECTORY ${FF_CORE_DIR}/interface/
        DESTINATION include/findfriend/interface
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/
        DESTINATION lib
        FILES_MATCHING
        PATTERN "*.a"
        PATTERN "*.so")

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
        DESTINATION bin
        FILES_MATCHING
        PATTERN "*"
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ)

# ====================================================================
# 打印配置
# ====================================================================
message(STATUS "")
message(STATUS "==========================================")
message(STATUS "  FindFriend Build Configuration")
message(STATUS "==========================================")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Rust modules: ${FF_USE_RUST}")
message(STATUS "  Build CLI: ${FF_BUILD_CLI}")
message(STATUS "  Enable logging: ${FF_ENABLE_LOGGING}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "==========================================")
message(STATUS "")
