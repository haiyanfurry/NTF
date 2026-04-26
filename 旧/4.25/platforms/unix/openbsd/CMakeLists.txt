cmake_minimum_required(VERSION 3.10)

project(FindFriendBSD VERSION 4.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(COMMON_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../common_core)
set(COMMON_UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../common_ui)

include_directories(
    ${COMMON_CORE_DIR}
    ${COMMON_UI_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(${COMMON_CORE_DIR} common_core)
add_subdirectory(${COMMON_UI_DIR} common_ui)

add_executable(FindFriendBSD_OpenBSD
    main.c
    platform_specific.c
)

target_link_libraries(FindFriendBSD_OpenBSD
    FindFriendCore
    FindFriendUI
    pthread
    m
    crypto
    ssl
)

target_include_directories(FindFriendBSD_OpenBSD PRIVATE
    ${COMMON_CORE_DIR}
)

add_executable(FindFriendCLI_BSD
    ${COMMON_CORE_DIR}/core_cli.c
    platform_specific.c
)

target_link_libraries(FindFriendCLI_BSD
    FindFriendCore
    pthread
    m
    crypto
    ssl
)

target_include_directories(FindFriendCLI_BSD PRIVATE
    ${COMMON_CORE_DIR}
)

message(STATUS "OpenBSD Build Configuration")
message(STATUS "  Platform: OpenBSD")
message(STATUS "  Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
