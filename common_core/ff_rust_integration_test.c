// ====================================================================
// FindFriend C <-> Rust 混合架构集成测试
// 演示如何从 C 代码调用 Rust 模块
// ====================================================================

#include "ff_safe_utils.h"
#include "rust_modules/c_bindings/ff_rust_bindings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ====================================================================
// Rust 加密模块测试
// ====================================================================
static void test_rust_crypto(void) {
    FF_INFO("=== Testing Rust Crypto Module ===");
    
    FFCryptoErrorCode result = rust_crypto_init();
    if (result != FF_CRYPTO_OK) {
        FF_ERROR("rust_crypto_init failed: %s", rust_get_crypto_error_message(result));
        return;
    }
    
    FF_INFO("Rust crypto module initialized successfully");
    
    // SHA-256 测试
    uint8_t input[] = "Hello from Rust!";
    uint8_t hash[32];
    
    result = rust_sha256(input, sizeof(input) - 1, hash, sizeof(hash));
    if (result == FF_CRYPTO_OK) {
        FF_INFO("SHA-256 hash computed successfully");
        printf("  Hash: ");
        for (int i = 0; i < 32; i++) {
            printf("%02x", hash[i]);
        }
        printf("\n");
    } else {
        FF_ERROR("SHA-256 hash failed: %s", rust_get_crypto_error_message(result));
    }
    
    // 随机数生成测试
    uint8_t random_buf[16];
    result = rust_random_bytes(random_buf, sizeof(random_buf));
    if (result == FF_CRYPTO_OK) {
        FF_INFO("Random bytes generated successfully");
        printf("  Random: ");
        for (int i = 0; i < 16; i++) {
            printf("%02x", random_buf[i]);
        }
        printf("\n");
    }
    
    FF_INFO("=== Rust Crypto Test Complete ===\n");
}

// ====================================================================
// Rust 安全模块测试
// ====================================================================
static void test_rust_security(void) {
    FF_INFO("=== Testing Rust Security Module ===");
    
    FFSecurityErrorCode result = rust_security_init();
    if (result != FF_SECURITY_OK) {
        FF_ERROR("rust_security_init failed: %s", rust_get_security_error_message(result));
        return;
    }
    
    FF_INFO("Rust security module initialized successfully");
    
    // 邮箱验证测试
    const char* good_email = "user@example.com";
    const char* bad_email = "invalid-email";
    
    result = rust_validate_email(good_email);
    FF_INFO("Email '%s': %s", good_email, result == FF_SECURITY_OK ? "VALID" : "INVALID");
    
    result = rust_validate_email(bad_email);
    FF_INFO("Email '%s': %s", bad_email, result == FF_SECURITY_OK ? "VALID" : "INVALID");
    
    // SQL 注入测试
    const char* safe_str = "hello world";
    const char* danger_str = "UNION SELECT * FROM users";
    
    result = rust_check_sql_injection(safe_str);
    FF_INFO("SQL check '%s': %s", safe_str, result == FF_SECURITY_OK ? "SAFE" : "UNSAFE");
    
    result = rust_check_sql_injection(danger_str);
    FF_INFO("SQL check '%s': %s", danger_str, result == FF_SECURITY_OK ? "SAFE" : "UNSAFE");
    
    // XSS 注入测试
    const char* dangerous_xss = "<script>alert('xss')</script>";
    result = rust_check_xss_injection(dangerous_xss);
    FF_INFO("XSS check '%s': %s", dangerous_xss, result == FF_SECURITY_OK ? "SAFE" : "UNSAFE");
    
    // 字符串清理
    char sanitized[256];
    result = rust_sanitize_string(dangerous_xss, sanitized, sizeof(sanitized));
    FF_INFO("Sanitized string: %s", sanitized);
    
    FF_INFO("=== Rust Security Test Complete ===\n");
}

// ====================================================================
// Rust 网络模块测试
// ====================================================================
static void test_rust_network(void) {
    FF_INFO("=== Testing Rust Network Module ===");
    
    FFNetConfig config;
    memset(&config, 0, sizeof(config));
    config.host = "127.0.0.1";
    config.port = 8080;
    config.use_tls = false;
    config.buffer_size = 4096;
    
    FFNetErrorCode result = rust_network_init(&config);
    if (result != FF_NET_OK) {
        FF_ERROR("rust_network_init failed: %s", rust_get_error_message(result));
        return;
    }
    
    FF_INFO("Rust network module initialized successfully");
    
    bool is_init = rust_network_is_initialized();
    FF_INFO("Network initialized: %s", is_init ? "yes" : "no");
    
    // P2P 发现测试
    result = rust_p2p_discover();
    if (result == FF_NET_OK) {
        FF_INFO("P2P discovery started");
    }
    
    // 连接测试（模拟）
    uint64_t conn_id = 0;
    result = rust_connect_peer("test-peer", "127.0.0.1", 8081, &conn_id);
    if (result == FF_NET_OK) {
        FF_INFO("Connected to peer, conn_id=%llu", (unsigned long long)conn_id);
        
        uint8_t test_data[] = "Hello from C to Rust!";
        size_t sent = 0;
        
        result = rust_send_secure(conn_id, test_data, sizeof(test_data) - 1, &sent);
        if (result == FF_NET_OK) {
            FF_INFO("Sent %zu bytes securely", sent);
        }
        
        rust_disconnect_peer(conn_id);
        FF_INFO("Disconnected from peer");
    }
    
    FF_INFO("=== Rust Network Test Complete ===\n");
}

// ====================================================================
// C 安全辅助函数测试
// ====================================================================
static void test_safe_utils(void) {
    FF_INFO("=== Testing C Safe Utilities ===");
    
    // 安全字符串操作测试
    char dest[16];
    const char* src = "Hello, this is a long string that should be truncated!";
    
    size_t copied = ff_safe_strncpy(dest, src, sizeof(dest));
    FF_INFO("ff_safe_strncpy copied %zu bytes: '%s'", copied, dest);
    
    // 安全内存分配测试
    char* buffer = (char*)FF_MALLOC(1024);
    if (buffer) {
        FF_INFO("Allocated 1024 bytes buffer");
        ff_safe_strncpy(buffer, "Test buffer", 1024);
        FF_FREE(buffer);
    }
    
    FF_INFO("=== C Safe Utilities Test Complete ===\n");
}

// ====================================================================
// 主函数：运行所有测试
// ====================================================================
int main(int argc, char* argv[]) {
    FF_INFO("==========================================");
    FF_INFO("  FindFriend C <-> Rust Integration Test");
    FF_INFO("==========================================");
    
    // 设置日志级别
    ff_log_set_level(FF_LOG_LEVEL_DEBUG);
    
    FF_INFO("Starting integration tests...");
    
    // 运行各个模块测试
    test_safe_utils();
    test_rust_crypto();
    test_rust_security();
    test_rust_network();
    
    // 检查内存泄漏
    FF_INFO("Checking memory leaks...");
    ff_check_memory_leaks();
    
    FF_INFO("All tests completed!");
    FF_INFO("==========================================");
    
    return 0;
}
