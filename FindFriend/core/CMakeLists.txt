# FindFriend CMake 构建配置 - Core 模块

# ====================================================================
# 核心接口
# ====================================================================
set(FF_INTERFACE_SOURCES
    ${FF_INTERFACE_DIR}/ff_core_interface.h
    ${FF_INTERFACE_DIR}/ff_user_interface.h
    ${FF_INTERFACE_DIR}/ff_message_interface.h
    ${FF_INTERFACE_DIR}/ff_location_interface.h
    ${FF_INTERFACE_DIR}/ff_p2p_interface.h
    ${FF_INTERFACE_DIR}/ff_game_interface.h
    ${FF_INTERFACE_DIR}/ff_security_interface.h
)

add_library(ff_interface INTERFACE)
target_sources(ff_interface INTERFACE ${FF_INTERFACE_SOURCES})
target_include_directories(ff_interface INTERFACE ${FF_INTERFACE_DIR})

# ====================================================================
# 核心业务模块
# ====================================================================
add_library(ff_core STATIC
    core_init.c
    business/user_manager.c
    business/message_manager.c
    business/social_manager.c
    business/game_manager.c
    business/location_service.c
)

target_link_libraries(ff_core PUBLIC ff_interface ff_utils)

if(FF_ENABLE_LOGGING)
    target_compile_definitions(ff_core PRIVATE FF_ENABLE_LOGGING=1)
endif()

target_include_directories(ff_core PUBLIC
    ${FF_INTERFACE_DIR}
    ${CMAKE_SOURCE_DIR}
)

# ====================================================================
# P2P 模块
# ====================================================================
add_library(ff_p2p STATIC
    p2p/p2p.c
    p2p/node_discovery.c
    p2p/connection.c
    p2p/message_routing.c
    p2p/nat_traversal.c
)

target_link_libraries(ff_p2p PUBLIC ff_core)

# ====================================================================
# 数据模块
# ====================================================================
add_library(ff_data STATIC
    data/data_store.c
    data/cache_manager.c
)

target_link_libraries(ff_data PUBLIC ff_core)

# ====================================================================
# 工具模块
# ====================================================================
add_library(ff_utils STATIC
    utils/safe_string.c
    utils/memory_pool.c
    utils/log_manager.c
    utils/config_manager.c
)

target_link_libraries(ff_utils PUBLIC ff_core)

# ====================================================================
# 主核心库
# ====================================================================
add_library(FindFriendCore STATIC
    $<TARGET_OBJECTS:ff_core>
    $<TARGET_OBJECTS:ff_p2p>
    $<TARGET_OBJECTS:ff_data>
    $<TARGET_OBJECTS:ff_utils>
)

target_link_libraries(FindFriendCore PUBLIC
    ff_core
    ff_p2p
    ff_data
    ff_utils
    pthread
    m
)

if(FF_ENABLE_LOGGING)
    target_compile_definitions(FindFriendCore PRIVATE FF_LOG_ENABLED=1)
endif()

message(STATUS "Core module built successfully")
