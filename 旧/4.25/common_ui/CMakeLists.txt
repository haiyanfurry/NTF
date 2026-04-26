# common_ui CMakeLists.txt

# 收集所有源文件
file(GLOB_RECURSE UI_SOURCES
    "*.c"
    "*.cpp"
)

# 收集所有头文件
file(GLOB_RECURSE UI_HEADERS
    "*.h"
    "*.hpp"
)

# 创建静态库
add_library(FindFriendUI STATIC
    ${UI_SOURCES}
    ${UI_HEADERS}
)

# 包含目录
target_include_directories(FindFriendUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/login
    ${CMAKE_CURRENT_SOURCE_DIR}/main
    ${CMAKE_CURRENT_SOURCE_DIR}/friend_list
    ${CMAKE_CURRENT_SOURCE_DIR}/chat
    ${CMAKE_CURRENT_SOURCE_DIR}/map
)

# 编译选项
target_compile_options(FindFriendUI PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# 链接库
if(WIN32)
    target_link_libraries(FindFriendUI PRIVATE
        ws2_32
    )
elseif(UNIX)
    target_link_libraries(FindFriendUI PRIVATE
        pthread
    )
endif()