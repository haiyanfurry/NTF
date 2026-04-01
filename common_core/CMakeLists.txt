# common_core CMakeLists.txt

# 收集所有源文件
file(GLOB_RECURSE CORE_SOURCES
    "*.c"
)

# 收集所有头文件
file(GLOB_RECURSE CORE_HEADERS
    "*.h"
)

# 创建静态库
add_library(FindFriendCore STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# 包含目录
target_include_directories(FindFriendCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/business
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/security
    ${CMAKE_CURRENT_SOURCE_DIR}/tls
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/util
    ${CMAKE_CURRENT_SOURCE_DIR}/bridge
)

# 编译选项
target_compile_options(FindFriendCore PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# 链接库
if(WIN32)
    target_link_libraries(FindFriendCore PRIVATE
        ws2_32
        crypto
        ssl
    )
elseif(UNIX)
    target_link_libraries(FindFriendCore PRIVATE
        pthread
        crypto
        ssl
    )
endif()