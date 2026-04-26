// ====================================================================
// FindFriend 安全接口
// 安全模块：输入验证、内容检测、安全策略
// ====================================================================

#ifndef FF_SECURITY_INTERFACE_H
#define FF_SECURITY_INTERFACE_H

#include "ff_core_interface.h"
#include <stdint.h>

// ====================================================================
// 安全事件类型
// ====================================================================
typedef enum FFSecurityEventType {
    FF_SECURITY_SQL_INJECTION = 1,
    FF_SECURITY_XSS_INJECTION = 2,
    FF_SECURITY_JSON_INJECTION = 3,
    FF_SECURITY_XML_INJECTION = 4,
    FF_SECURITY_RATE_LIMIT = 5,
    FF_SECURITY_SUSPICIOUS_LOGIN = 6,
    FF_SECURITY_FAKE_LOCATION = 7,
    FF_SECURITY_BOT_BEHAVIOR = 8,
    FF_SECURITY_DOS_ATTACK = 9,
    FF_SECURITY_MITM_ATTACK = 10
} FFSecurityEventType;

typedef struct FFSecurityEvent {
    FFSecurityEventType type;
    uint32_t uid;
    char description[512];
    char details[1024];
    uint64_t timestamp;
    bool handled;
} FFSecurityEvent;

// ====================================================================
// 设备指纹
// ====================================================================
typedef struct FFDeviceFingerprint {
    char device_id[64];
    char device_model[128];
    char os_version[64];
    char app_version[32];
    char signature[256];
    uint64_t created_at;
} FFDeviceFingerprint;

// ====================================================================
// 安全验证接口
// ====================================================================

// 输入验证
FFResult ff_security_validate_email(const char* email);
FFResult ff_security_validate_username(const char* username);
FFResult ff_security_validate_password(const char* password);
FFResult ff_security_validate_phone(const char* phone);

// 防注入检测
FFResult ff_security_check_sql_injection(const char* input);
FFResult ff_security_check_xss_injection(const char* input);
FFResult ff_security_check_json(const char* json_str);
FFResult ff_security_check_xml(const char* xml_str);

// 字符串清理
FFResult ff_security_sanitize_string(const char* input, char* output, size_t output_size);

// 内容检测（Rust 安全模块）
FFResult ff_security_check_message_content(const char* content);
FFResult ff_security_check_media_file(const char* file_path);
FFResult ff_security_check_game_action(const char* action_json);

// ====================================================================
// 异常行为检测
// ====================================================================
FFResult ff_security_check_rate_limit(uint32_t uid, const char* action_type, uint32_t window_seconds, uint32_t max_count);
FFResult ff_security_is_suspicious_user(uint32_t uid, bool* is_suspicious);
FFResult ff_security_report_user_action(uint32_t uid, const char* action_type, const char* details);
FFResult ff_security_restrict_user(uint32_t uid, FFAsyncCallback callback, void* user_data);

// 防虚假定位
FFResult ff_security_detect_fake_location(double lat, double lon, uint32_t uid, bool* is_fake);
FFResult ff_security_validate_location_consistency(uint32_t uid, double lat, double lon, uint64_t timestamp);

// 防爬虫
FFResult ff_security_check_crawler_behavior(uint32_t uid, bool* is_crawler);
FFResult ff_security_block_crawler(uint32_t uid);

// ====================================================================
// 设备指纹（Rust 安全模块）
// ====================================================================
FFResult ff_security_get_device_fingerprint(FFDeviceFingerprint** fingerprint);
FFResult ff_security_verify_device(const FFDeviceFingerprint* fingerprint, bool* is_valid);
FFResult ff_security_update_device_fingerprint(const FFDeviceFingerprint* fingerprint);

// ====================================================================
// 加密接口（Rust 安全模块）
// ====================================================================
FFResult ff_security_encrypt_data(const uint8_t* plaintext, size_t plain_len, uint8_t* ciphertext, size_t* cipher_len);
FFResult ff_security_decrypt_data(const uint8_t* ciphertext, size_t cipher_len, uint8_t* plaintext, size_t* plain_len);
FFResult ff_security_hash_data(const uint8_t* data, size_t data_len, uint8_t* hash, size_t* hash_len);
FFResult ff_security_generate_random_bytes(uint8_t* buffer, size_t len);

// ====================================================================
// MITM 防护（Rust 安全模块）
// ====================================================================
FFResult ff_security_check_certificate(const char* hostname, const uint8_t* cert_data, size_t cert_len, bool* is_valid);
FFResult ff_security_verify_signature(const uint8_t* data, size_t data_len, const uint8_t* signature, size_t sig_len, bool* is_valid);

// ====================================================================
// 数据完整性（Rust 安全模块）
// ====================================================================
FFResult ff_security_compute_integrity_check(const uint8_t* data, size_t data_len, uint8_t* checksum);
FFResult ff_security_verify_integrity_check(const uint8_t* data, size_t data_len, const uint8_t* checksum);

// ====================================================================
// 安全日志和告警
// ====================================================================
FFResult ff_security_log_event(FFSecurityEventType type, uint32_t uid, const char* description);
FFResult ff_security_get_events(FFSecurityEvent** events, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_security_clear_events(void);
FFResult ff_security_trigger_alert(const char* message);

// 内存释放
void ff_security_event_free(FFSecurityEvent* event);
void ff_security_event_list_free(FFSecurityEvent* events, size_t count);
void ff_security_fingerprint_free(FFDeviceFingerprint* fingerprint);

#endif // FF_SECURITY_INTERFACE_H
