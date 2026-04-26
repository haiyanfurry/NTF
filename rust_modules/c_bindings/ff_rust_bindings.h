// ====================================================================
// FindFriend C <-> Rust FFI 绑定头文件
// 此文件由 Cbindgen 自动生成或手动维护
// ====================================================================

#ifndef FF_RUST_BINDINGS_H
#define FF_RUST_BINDINGS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ====================================================================
// 统一错误码定义
// ====================================================================
typedef enum FFErrorCode {
    FF_OK = 0,
    FF_ERROR_INVALID_PARAM = -1,
    FF_ERROR_NETWORK = -2,
    FF_ERROR_TIMEOUT = -3,
    FF_ERROR_PERMISSION = -4,
    FF_ERROR_MEMORY = -5,
    FF_ERROR_ALREADY_INITIALIZED = -6,
    FF_ERROR_NOT_INITIALIZED = -7,
    FF_ERROR_ENCRYPTION = -8,
    FF_ERROR_CONNECTION_CLOSED = -9,
    FF_ERROR_BUFFER_TOO_SMALL = -10,
    FF_ERROR_UNKNOWN = -99
} FFErrorCode;

// ====================================================================
// rust_network.h - Rust 网络模块
// ====================================================================

typedef enum FFNetErrorCode {
    FF_NET_OK = 0,
    FF_NET_INVALID_PARAM = -1,
    FF_NET_ERROR = -2,
    FF_NET_TIMEOUT = -3,
    FF_NET_PERMISSION = -4,
    FF_NET_MEMORY = -5,
    FF_NET_ALREADY_INITIALIZED = -6,
    FF_NET_NOT_INITIALIZED = -7,
    FF_NET_ENCRYPTION_ERROR = -8,
    FF_NET_CONNECTION_CLOSED = -9,
    FF_NET_BUFFER_TOO_SMALL = -10,
    FF_NET_UNKNOWN = -99
} FFNetErrorCode;

typedef struct FFNetConfig {
    const char* host;
    uint16_t port;
    bool use_tls;
    uint32_t timeout_ms;
    size_t buffer_size;
    bool enable_encryption;
    const char* peer_id;
} FFNetConfig;

FFNetErrorCode rust_network_init(const FFNetConfig* config);
FFNetErrorCode rust_network_destroy(void);
FFNetErrorCode rust_send_secure(uint64_t conn_id, const uint8_t* data, size_t data_len, size_t* sent);
FFNetErrorCode rust_receive_secure(uint64_t conn_id, uint8_t* buffer, size_t buffer_size, size_t* received);
FFNetErrorCode rust_connect_peer(const char* peer_id, const char* host, uint16_t port, uint64_t* conn_id);
FFNetErrorCode rust_disconnect_peer(uint64_t conn_id);
FFNetErrorCode rust_udp_send(const char* dest_addr, uint16_t dest_port, const uint8_t* data, size_t data_len);
FFNetErrorCode rust_p2p_discover(void);
bool rust_network_is_initialized(void);
const char* rust_get_error_message(FFNetErrorCode error_code);

// ====================================================================
// rust_crypto.h - Rust 加密模块
// ====================================================================

typedef enum FFCryptoErrorCode {
    FF_CRYPTO_OK = 0,
    FF_CRYPTO_INVALID_PARAM = -1,
    FF_CRYPTO_KEY_ERROR = -2,
    FF_CRYPTO_ENCRYPT_ERROR = -3,
    FF_CRYPTO_DECRYPT_ERROR = -4,
    FF_CRYPTO_HASH_ERROR = -5,
    FF_CRYPTO_RANDOM_ERROR = -6,
    FF_CRYPTO_BUFFER_TOO_SMALL = -10,
    FF_CRYPTO_NOT_INITIALIZED = -7,
    FF_CRYPTO_UNKNOWN_ERROR = -99
} FFCryptoErrorCode;

FFCryptoErrorCode rust_crypto_init(void);
FFCryptoErrorCode rust_crypto_destroy(void);
FFCryptoErrorCode rust_sha256(const uint8_t* input, size_t input_len, uint8_t* output, size_t output_size);
FFCryptoErrorCode rust_sha512(const uint8_t* input, size_t input_len, uint8_t* output, size_t output_size);
FFCryptoErrorCode rust_hmac_sha256(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* output, size_t output_size);
FFCryptoErrorCode rust_encrypt_aes256_gcm(
    const uint8_t* plaintext, size_t plaintext_len,
    const uint8_t* associated_data, size_t ad_len,
    const uint8_t* key, size_t key_len,
    const uint8_t* nonce, size_t nonce_len,
    uint8_t* ciphertext, size_t* ciphertext_size
);
FFCryptoErrorCode rust_decrypt_aes256_gcm(
    const uint8_t* ciphertext, size_t ciphertext_len,
    const uint8_t* associated_data, size_t ad_len,
    const uint8_t* key, size_t key_len,
    const uint8_t* nonce, size_t nonce_len,
    uint8_t* plaintext, size_t* plaintext_size
);
FFCryptoErrorCode rust_random_bytes(uint8_t* buffer, size_t buffer_size);
FFCryptoErrorCode rust_secure_zero(uint8_t* buffer, size_t buffer_size);
const char* rust_get_crypto_error_message(FFCryptoErrorCode code);
bool rust_crypto_is_initialized(void);

// ====================================================================
// rust_security.h - Rust 安全模块
// ====================================================================

typedef enum FFSecurityErrorCode {
    FF_SECURITY_OK = 0,
    FF_SECURITY_INVALID_INPUT = -1,
    FF_SECURITY_SQL_INJECTION = -2,
    FF_SECURITY_XSS_INJECTION = -3,
    FF_SECURITY_JSON_INJECTION = -4,
    FF_SECURITY_XML_INJECTION = -5,
    FF_SECURITY_RATE_LIMIT = -6,
    FF_SECURITY_INVALID_JSON = -7,
    FF_SECURITY_INVALID_XML = -8,
    FF_SECURITY_INVALID_EMAIL = -9,
    FF_SECURITY_INVALID_PHONE = -10,
    FF_SECURITY_INVALID_USERNAME = -11,
    FF_SECURITY_INVALID_PASSWORD = -12,
    FF_SECURITY_TOO_LONG = -13,
    FF_SECURITY_BLACKLISTED = -14,
    FF_SECURITY_UNKNOWN = -99
} FFSecurityErrorCode;

FFSecurityErrorCode rust_security_init(void);
FFSecurityErrorCode rust_security_destroy(void);
bool rust_security_is_initialized(void);

FFSecurityErrorCode rust_validate_email(const char* email);
FFSecurityErrorCode rust_validate_username(const char* username);
FFSecurityErrorCode rust_validate_password(const char* password);

FFSecurityErrorCode rust_check_sql_injection(const char* input);
FFSecurityErrorCode rust_check_xss_injection(const char* input);

FFSecurityErrorCode rust_sanitize_string(const char* input, char* output, size_t output_size);

FFSecurityErrorCode rust_validate_json(const char* json_str);
FFSecurityErrorCode rust_validate_xml(const char* xml_str);

const char* rust_get_security_error_message(FFSecurityErrorCode code);

// ====================================================================
// 辅助函数：C 代码质量优化
// ====================================================================

// 日志宏（支持多等级）
#define FF_LOG_LEVEL_DEBUG 0
#define FF_LOG_LEVEL_INFO 1
#define FF_LOG_LEVEL_WARN 2
#define FF_LOG_LEVEL_ERROR 3

// 统一日志系统（使用 Rust 或 C 自己的日志）
void ff_log(int level, const char* fmt, ...);

// 安全字符串函数
size_t ff_safe_strncpy(char* dest, const char* src, size_t dest_size);
size_t ff_safe_strncat(char* dest, const char* src, size_t dest_size);

// 内存安全辅助
void* ff_safe_malloc(size_t size, const char* filename, int line);
void ff_safe_free(void** ptr);

#define FF_MALLOC(size) ff_safe_malloc(size, __FILE__, __LINE__)
#define FF_FREE(ptr) ff_safe_free((void**)&(ptr))

#ifdef __cplusplus
}
#endif

#endif // FF_RUST_BINDINGS_H
