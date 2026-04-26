#ifndef MEMORY_ENCRYPT_H
#define MEMORY_ENCRYPT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 内存加密结构体
typedef struct {
    uint8_t* encrypted_data;
    size_t length;
    uint8_t key[32];
    uint8_t iv[16];
} EncryptedMemory;

// 初始化内存加密模块
bool memory_encrypt_init(void);

// 加密内存数据
EncryptedMemory* memory_encrypt(const uint8_t* data, size_t length);

// 解密内存数据
bool memory_decrypt(EncryptedMemory* encrypted, uint8_t* output, size_t* output_length);

// 安全释放加密内存
void memory_encrypt_free(EncryptedMemory* encrypted);

// 加密字符串
EncryptedMemory* memory_encrypt_string(const char* str);

// 解密字符串
char* memory_decrypt_string(EncryptedMemory* encrypted);

// 安全清除内存
bool memory_secure_clear(uint8_t* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_ENCRYPT_H