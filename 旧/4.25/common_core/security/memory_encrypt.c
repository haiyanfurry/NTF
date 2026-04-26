#include "memory_encrypt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

// 初始化内存加密模块
bool memory_encrypt_init(void) {
    // 初始化OpenSSL
    SSL_load_error_strings();
    SSL_library_init();
    return true;
}

// 加密内存数据
EncryptedMemory* memory_encrypt(const uint8_t* data, size_t length) {
    if (!data || length == 0) {
        return NULL;
    }
    
    EncryptedMemory* encrypted = (EncryptedMemory*)malloc(sizeof(EncryptedMemory));
    if (!encrypted) {
        fprintf(stderr, "Error allocating memory for encrypted data\n");
        return NULL;
    }
    
    // 生成随机密钥和IV
    if (RAND_bytes(encrypted->key, 32) != 1 || RAND_bytes(encrypted->iv, 16) != 1) {
        fprintf(stderr, "Error generating random key and IV\n");
        free(encrypted);
        return NULL;
    }
    
    // 创建EVP加密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Error creating EVP context\n");
        free(encrypted);
        return NULL;
    }
    
    // 初始化加密操作
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encrypted->key, encrypted->iv) != 1) {
        fprintf(stderr, "Error initializing encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }
    
    // 计算加密后的数据长度
    size_t encrypted_length = length + EVP_CIPHER_block_size(EVP_aes_256_cbc());
    encrypted->encrypted_data = (uint8_t*)malloc(encrypted_length);
    if (!encrypted->encrypted_data) {
        fprintf(stderr, "Error allocating memory for encrypted data\n");
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }
    
    // 执行加密
    int out_len = 0;
    if (EVP_EncryptUpdate(ctx, encrypted->encrypted_data, &out_len, data, length) != 1) {
        fprintf(stderr, "Error encrypting data\n");
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted->encrypted_data);
        free(encrypted);
        return NULL;
    }
    
    // 完成加密
    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx, encrypted->encrypted_data + out_len, &final_len) != 1) {
        fprintf(stderr, "Error finalizing encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted->encrypted_data);
        free(encrypted);
        return NULL;
    }
    
    encrypted->length = out_len + final_len;
    
    // 清理上下文
    EVP_CIPHER_CTX_free(ctx);
    
    return encrypted;
}

// 解密内存数据
bool memory_decrypt(EncryptedMemory* encrypted, uint8_t* output, size_t* output_length) {
    if (!encrypted || !output || !output_length) {
        return false;
    }
    
    // 创建EVP解密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Error creating EVP context\n");
        return false;
    }
    
    // 初始化解密操作
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encrypted->key, encrypted->iv) != 1) {
        fprintf(stderr, "Error initializing decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 执行解密
    int out_len = 0;
    if (EVP_DecryptUpdate(ctx, output, &out_len, encrypted->encrypted_data, encrypted->length) != 1) {
        fprintf(stderr, "Error decrypting data\n");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 完成解密
    int final_len = 0;
    if (EVP_DecryptFinal_ex(ctx, output + out_len, &final_len) != 1) {
        fprintf(stderr, "Error finalizing decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    *output_length = out_len + final_len;
    
    // 清理上下文
    EVP_CIPHER_CTX_free(ctx);
    
    return true;
}

// 安全释放加密内存
void memory_encrypt_free(EncryptedMemory* encrypted) {
    if (encrypted) {
        if (encrypted->encrypted_data) {
            // 安全清除加密数据
            memory_secure_clear(encrypted->encrypted_data, encrypted->length);
            free(encrypted->encrypted_data);
        }
        // 安全清除密钥和IV
        memory_secure_clear(encrypted->key, 32);
        memory_secure_clear(encrypted->iv, 16);
        free(encrypted);
    }
}

// 加密字符串
EncryptedMemory* memory_encrypt_string(const char* str) {
    if (!str) {
        return NULL;
    }
    return memory_encrypt((const uint8_t*)str, strlen(str) + 1);
}

// 解密字符串
char* memory_decrypt_string(EncryptedMemory* encrypted) {
    if (!encrypted) {
        return NULL;
    }
    
    char* str = (char*)malloc(encrypted->length);
    if (!str) {
        fprintf(stderr, "Error allocating memory for decrypted string\n");
        return NULL;
    }
    
    size_t length = 0;
    if (!memory_decrypt(encrypted, (uint8_t*)str, &length)) {
        free(str);
        return NULL;
    }
    
    return str;
}

// 安全清除内存
bool memory_secure_clear(uint8_t* data, size_t length) {
    if (!data) {
        return false;
    }
    
    // 使用memset_s进行安全清除
    memset(data, 0, length);
    
    return true;
}

// 清理内存加密模块
void memory_encrypt_cleanup(void) {
    // 无资源需要释放
}