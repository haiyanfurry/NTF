# FindFriend CLI 工具 CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

project(ff_cli C)

set(CMAKE_C_STANDARD 99)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 源文件
set(CLI_SOURCES
    cli_main.c
    commands.c
    input_handler.c
    display.c
)

# CLI 工具主可执行文件
add_executable(FindFriendCLI ${CLI_SOURCES})
target_link_libraries(FindFriendCLI
    FindFriendCore
    pthread
    m
)
target_include_directories(FindFriendCLI PRIVATE
    ${CMAKE_SOURCE_DIR}/core/interface
    ${CMAKE_SOURCE_DIR}
)
target_compile_options(FindFriendCLI PRIVATE
    -Wall
    -Wextra
    -O2
)

# 安装
install(TARGETS FindFriendCLI DESTINATION bin)

# P2P 测试工具
add_executable(ff_p2p_test p2p_test.c)
target_link_libraries(ff_p2p_test
    FindFriendCore
    pthread
    m
)
target_include_directories(ff_p2p_test PRIVATE
    ${CMAKE_SOURCE_DIR}/core/interface
    ${CMAKE_SOURCE_DIR}
)
target_compile_options(ff_p2p_test PRIVATE
    -Wall
    -Wextra
    -O2
)
install(TARGETS ff_p2p_test DESTINATION bin)

message(STATUS "CLI tool configuration complete")
