# FindFriend BSD 平台 CMakeLists.txt

# 平台特定配置
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(BSD_PLATFORM "freebsd")
elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    set(BSD_PLATFORM "netbsd")
elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    set(BSD_PLATFORM "openbsd")
else()
    message(FATAL_ERROR "Unknown BSD platform")
endif()

# 源文件
set(BSD_SOURCES
    ${BSD_PLATFORM}/main.c
    ${BSD_PLATFORM}/platform_specific.c
    ${BSD_PLATFORM}/network.c
    ${BSD_PLATFORM}/security.c
    ${BSD_PLATFORM}/system.c
)

# 可执行文件
add_executable(FindFriendBSD
    ${BSD_SOURCES}
)

# 链接库
target_link_libraries(FindFriendBSD
    FindFriendCore
    pthread
    m
    rt
)

# FreeBSD 特定链接
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    target_link_libraries(FindFriendBSD
        execinfo
    )
endif()

# 包含目录
target_include_directories(FindFriendBSD PRIVATE
    ${CMAKE_SOURCE_DIR}/core/interface
    ${BSD_PLATFORM}
)

# 编译选项
target_compile_options(FindFriendBSD PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O2
    -D${BSD_PLATFORM^^}
)

# 安装
install(TARGETS FindFriendBSD DESTINATION bin)

message(STATUS "BSD platform ${BSD_PLATFORM} configuration complete")
