# FindFriend Linux 平台 CMakeLists.txt

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # 源文件
    set(LINUX_SOURCES
        main.c
        platform_specific.c
    )
    
    # 可执行文件
    add_executable(FindFriendLinux
        ${LINUX_SOURCES}
    )
    
    # 链接库
    target_link_libraries(FindFriendLinux
        FindFriendCore
        pthread
        m
    )
    
    # 包含目录
    target_include_directories(FindFriendLinux PRIVATE
        ${CMAKE_SOURCE_DIR}/core/interface
        ${CMAKE_SOURCE_DIR}
    )
    
    # 编译选项
    target_compile_options(FindFriendLinux PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -O2
    )
    
    # 安装
    install(TARGETS FindFriendLinux DESTINATION bin)
    
    message(STATUS "Linux platform configuration complete")
endif()
