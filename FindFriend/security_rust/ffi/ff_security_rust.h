// ====================================================================
// FindFriend Rust 安全模块 FFI 头文件
// C 代码通过这些接口调用 Rust 安全功能
// ====================================================================

#ifndef FF_SECURITY_RUST_H
#define FF_SECURITY_RUST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ====================================================================
// 错误码
// ====================================================================
typedef enum FFSRResult {
    FFSR_OK = 0,
    FFSR_ERROR_INVALID_PARAM = -1,
    FFSR_ERROR_CRYPTO = -2,
    FFSR_ERROR_HASH = -3,
    FFSR_ERROR_VALIDATION = -4,
    FFSR_ERROR_DEVICE = -5,
    FFSR_ERROR_MEMORY = -6,
    FFSR_ERROR_TIMEOUT = -7,
    FFSR_ERROR_UNKNOWN = -99
} FFSRResult;

// ====================================================================
// Rust 安全模块初始化
// ====================================================================
bool ff_sr_is_initialized(void);
FFSRResult ff_sr_init(void);
FFSRResult ff_sr_shutdown(void);

// ====================================================================
// 内存加密
// ====================================================================

// 加密内存区域
// buffer: 要加密的内存区域
// len: 内存区域长度（必须是 16 的倍数）
// key_id: 密钥标识符
FFSRResult ff_sr_memory_encrypt(uint8_t* buffer, size_t len, uint32_t key_id);

// 解密内存区域
FFSRResult ff_sr_memory_decrypt(uint8_t* buffer, size_t len, uint32_t key_id);

// 安全内存分配（返回加密后的内存）
// 返回的内存需要用 ff_sr_secure_free 释放
void* ff_sr_secure_malloc(size_t size);

// 安全内存释放（加密清零）
void ff_sr_secure_free(void* ptr, size_t size);

// ====================================================================
// 设备指纹
// ====================================================================

// 生成设备指纹
// fingerprint: 输出缓冲区（应至少 256 字节）
// len: 输出缓冲区长度
FFSRResult ff_sr_generate_device_fingerprint(char* fingerprint, size_t len);

// 验证设备指纹
// fingerprint: 要验证的指纹字符串
bool ff_sr_verify_device_fingerprint(const char* fingerprint);

// 获取设备指纹签名
// fingerprint: 指纹字符串
// signature: 输出签名（应至少 64 字节）
// sig_len: 签名长度
FFSRResult ff_sr_sign_fingerprint(const char* fingerprint, uint8_t* signature, size_t* sig_len);

// 验证设备指纹签名
FFSRResult ff_sr_verify_fingerprint_signature(const char* fingerprint, const uint8_t* signature, size_t sig_len);

// ====================================================================
// MITM 防护
// ====================================================================

// 证书验证
// hostname: 服务器主机名
// cert_pem: PEM 格式证书
// cert_len: 证书长度
FFSRResult ff_sr_verify_certificate(const char* hostname, const char* cert_pem, size_t cert_len);

// 生成证书签名
// data: 要签名的数据
// data_len: 数据长度
// signature: 输出签名
// sig_len: 签名长度
FFSRResult ff_sr_generate_signature(const uint8_t* data, size_t data_len, uint8_t* signature, size_t* sig_len);

// 验证签名
FFSRResult ff_sr_verify_signature(const uint8_t* data, size_t data_len, const uint8_t* signature, size_t sig_len);

// 生成 TLS 会话密钥
FFSRResult ff_sr_generate_session_key(const char* hostname, uint8_t* key, size_t* key_len);

// ====================================================================
// 数据完整性校验
// ====================================================================

// 计算数据校验和（CRC64）
FFSRResult ff_sr_compute_checksum(const uint8_t* data, size_t data_len, uint64_t* checksum);

// 验证数据完整性
FFSRResult ff_sr_verify_checksum(const uint8_t* data, size_t data_len, uint64_t expected_checksum);

// 计算 HMAC
FFSRResult ff_sr_compute_hmac(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* hmac, size_t* hmac_len);

// 验证 HMAC
FFSRResult ff_sr_verify_hmac(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, const uint8_t* hmac, size_t hmac_len);

// ====================================================================
// 位置模糊化
// ====================================================================

// 位置模糊化（高斯模糊）
// exact_lat, exact_lon: 精确经纬度
// radius_meters: 模糊半径（米）
// out_lat, out_lon: 输出模糊经纬度
FFSRResult ff_sr_obfuscate_location(double exact_lat, double exact_lon, double radius_meters, double* out_lat, double* out_lon);

// 计算模糊距离（将精确距离转换为模糊距离）
FFSRResult ff_sr_fuzzy_distance(double exact_meters, double* fuzzy_meters);

// 验证位置合理性（防止虚假 GPS）
FFSRResult ff_sr_validate_location(double lat, double lon, double speed, double heading, bool* is_reasonable);

// ====================================================================
// 安全随机数
// ====================================================================

// 生成安全随机字节
FFSRResult ff_sr_random_bytes(uint8_t* buffer, size_t len);

// 生成加密安全的密钥
FFSRResult ff_sr_generate_key(uint32_t key_id, size_t key_len);

// 获取密钥（仅内部使用）
FFSRResult ff_sr_get_key(uint32_t key_id, uint8_t* key, size_t* key_len);

// ====================================================================
// 错误处理
// ====================================================================
const char* ff_sr_get_error_message(FFSRResult result);

#ifdef __cplusplus
}
#endif

#endif // FF_SECURITY_RUST_H
