// FindFriend 安全模块 FFI 接口
// Rust 安全模块与 C 核心模块之间的接口

#ifndef FF_SECURITY_FFI_H
#define FF_SECURITY_FFI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ====================================================================
// 初始化与清理
// ====================================================================

FFResult ff_security_init(void);

void ff_security_cleanup(void);

// ====================================================================
// Boot Check & 完整性
// ====================================================================

bool ff_security_boot_check(void);

bool ff_security_check_integrity(void);

// ====================================================================
// Root/Jailbreak 检测
// ====================================================================

bool ff_security_is_rooted(void);

bool ff_security_is_jailbroken(void);

// ====================================================================
// 注入检测
// ====================================================================

bool ff_security_check_injection(void);

// ====================================================================
// MITM 防护
// ====================================================================

bool ff_security_check_mitm(void);

bool ff_security_verify_certificate(const uint8_t* cert_data, size_t cert_len);

// ====================================================================
// 内存加密
// ====================================================================

void* ff_security_encrypt_memory(const uint8_t* input, size_t input_len,
                                 const uint8_t* key, size_t key_len);

void* ff_security_decrypt_memory(const uint8_t* input, size_t input_len,
                                 const uint8_t* key, size_t key_len);

// ====================================================================
// 设备指纹
// ====================================================================

FFResult ff_security_get_device_fingerprint(uint8_t* fingerprint, size_t* fingerprint_len);

// ====================================================================
// 输入验证
// ====================================================================

bool ff_security_validate_email(const char* email);

bool ff_security_validate_username(const char* username);

bool ff_security_detect_sql_injection(const char* input);

bool ff_security_detect_xss(const char* input);

// ====================================================================
// 反爬虫
// ====================================================================

bool ff_security_detect_crawler(uint32_t uid, uint32_t query_count, uint32_t time_window);

void ff_security_add_query(uint32_t uid);

void ff_security_reset_user(uint32_t uid);

// ====================================================================
// 反 DoS
// ====================================================================

bool ff_security_detect_dos(const char* ip, uint32_t request_count, uint32_t time_window);

bool ff_security_is_ip_blocked(const char* ip);

void ff_security_block_ip(const char* ip);

void ff_security_unblock_ip(const char* ip);

// ====================================================================
// 反虚假 GPS
// ====================================================================

bool ff_security_detect_fake_location(double lat, double lon, double speed,
                                     double last_lat, double last_lon, uint32_t time_diff);

bool ff_security_add_location(uint32_t uid, double lat, double lon, double speed);

// ====================================================================
// 日志和审计
// ====================================================================

typedef enum {
    FF_SECURITY_LEVEL_LOW = 0,
    FF_SECURITY_LEVEL_MEDIUM = 1,
    FF_SECURITY_LEVEL_HIGH = 2,
    FF_SECURITY_LEVEL_CRITICAL = 3
} FFSecturityLevel;

typedef enum {
    FF_SECURITY_EVENT_INFO = 0,
    FF_SECURITY_EVENT_WARNING = 1,
    FF_SECURITY_EVENT_ERROR = 2,
    FF_SECURITY_EVENT_CRITICAL = 3
} FFSecurityEventType;

void ff_security_log_event(FFSecurityEventType event_type, const char* message);

FFResult ff_security_get_status(FFSecurityStatus* status);

typedef struct {
    int overall;
    bool root_detected;
    bool jailbreak_detected;
    bool mitm_detected;
    bool dos_attack;
    bool fake_gps;
    bool crawler_detected;
    uint32_t blocked_ip_count;
    uint32_t detected_threat_count;
} FFSecurityStatus;

// ====================================================================
// 内存安全
// ====================================================================

void* ff_security_secure_alloc(size_t size);

void ff_security_secure_free(void* ptr, size_t size);

void ff_security_secure_zero(void* ptr, size_t size);

// ====================================================================
// XML 安全
// ====================================================================

bool ff_security_validate_xml(const char* xml_data, size_t xml_len);

bool ff_security_check_xml_injection(const char* xml_data, size_t xml_len);

// ====================================================================
// 数据包安全
// ====================================================================

bool ff_security_validate_packet(const uint8_t* packet, size_t packet_len);

FFResult ff_security_encrypt_packet(const uint8_t* input, size_t input_len,
                                    uint8_t* output, size_t* output_len,
                                    const uint8_t* key, size_t key_len);

FFResult ff_security_decrypt_packet(const uint8_t* input, size_t input_len,
                                  uint8_t* output, size_t* output_len,
                                  const uint8_t* key, size_t key_len);

#ifdef __cplusplus
}
#endif

#endif // FF_SECURITY_FFI_H
