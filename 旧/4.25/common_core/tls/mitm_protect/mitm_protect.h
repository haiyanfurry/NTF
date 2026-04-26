#ifndef MITM_PROTECT_H
#define MITM_PROTECT_H

#include <stddef.h>

// 检查证书指纹
int check_certificate_fingerprint(const char* fingerprint);

// 检查是否使用代理
int check_proxy_hijack(void);

// 检查 SSL 剥离
int check_ssl_stripping(const char* url);

// 检查数据篡改
int check_data_tampering(const char* original_data, const char* received_data, size_t length);

// 综合防中间人攻击检查
int mitm_protect_check(const char* fingerprint, const char* url, const char* original_data, const char* received_data, size_t length);

#endif // MITM_PROTECT_H