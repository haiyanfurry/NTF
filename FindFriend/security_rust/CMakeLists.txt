# FindFriend 安全模块 CMakeLists.txt

if(FF_USE_RUST)
    # 构建 Rust 库
    add_custom_target(security_rust ALL
        COMMAND ${CARGO_EXECUTABLE} build --release
        WORKING_DIRECTORY ${FF_SECURITY_DIR}
        COMMENT "Building Rust security module"
    )
    
    # 复制 Rust 库到输出目录
    add_custom_command(TARGET security_rust POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${FF_SECURITY_DIR}/target/release/libfindfriend_security.a
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfindfriend_security.a
        COMMENT "Copying Rust security library"
    )
    
    # 包含 FFI 头文件
    install(DIRECTORY ${FF_SECURITY_DIR}/ffi/
            DESTINATION include/findfriend/security
            FILES_MATCHING PATTERN "*.h")
    
    message(STATUS "Rust security module configured")
endif()
