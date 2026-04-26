// 定义_GNU_SOURCE以启用M_PI等数学常量
#define _GNU_SOURCE

#include "location_obfuscation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

// 地球半径（米）
#define EARTH_RADIUS 6371000.0

// 内部存储的真实位置（线程安全）
static PreciseLocation g_precise_location = {0};
static pthread_mutex_t g_location_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool g_initialized = false;

// 设备特定密钥（用于加密）
static uint8_t g_device_key[32] = {0};

// 初始化模糊定位模块
bool location_obfuscation_init(void) {
    pthread_mutex_lock(&g_location_mutex);
    
    if (g_initialized) {
        pthread_mutex_unlock(&g_location_mutex);
        return true;
    }
    
    // 生成设备特定密钥
    if (RAND_bytes(g_device_key, sizeof(g_device_key)) != 1) {
        pthread_mutex_unlock(&g_location_mutex);
        return false;
    }
    
    // 初始化位置为零
    memset(&g_precise_location, 0, sizeof(g_precise_location));
    g_precise_location.nonce = 0;
    
    g_initialized = true;
    
    pthread_mutex_unlock(&g_location_mutex);
    
    printf("[Location] Obfuscation module initialized\n");
    return true;
}

// 清理模糊定位模块
void location_obfuscation_cleanup(void) {
    pthread_mutex_lock(&g_location_mutex);
    
    // 安全清除位置数据
    location_secure_clear(&g_precise_location);
    memset(g_device_key, 0, sizeof(g_device_key));
    g_initialized = false;
    
    pthread_mutex_unlock(&g_location_mutex);
    
    printf("[Location] Obfuscation module cleaned up\n");
}

// 度转弧度
static double deg_to_rad(double deg) {
    return deg * M_PI / 180.0;
}

// 弧度转度
static double rad_to_deg(double rad) {
    return rad * 180.0 / M_PI;
}

// 生成高斯分布随机数（Box-Muller变换）
static double gaussian_random(double mean, double stddev) {
    static double spare;
    static bool has_spare = false;
    
    if (has_spare) {
        has_spare = false;
        return spare * stddev + mean;
    }
    
    double u, v, s;
    do {
        u = (double)rand() / RAND_MAX * 2.0 - 1.0;
        v = (double)rand() / RAND_MAX * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    
    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    has_spare = true;
    
    return u * s * stddev + mean;
}

// 模糊化坐标（核心算法）
void location_obfuscate_coordinates(double lat, double lon, double* out_lat, double* out_lon) {
    if (!out_lat || !out_lon) return;
    
    // 使用高斯分布在500米半径内随机偏移
    // 标准差设为 500/3 ≈ 167米，保证99.7%的点在500米内
    double stddev = OBFUSCATION_RADIUS_METERS / 3.0;
    
    // 生成随机偏移（米）
    double offset_north = gaussian_random(0, stddev);
    double offset_east = gaussian_random(0, stddev);
    
    // 转换为度
    double lat_rad = deg_to_rad(lat);
    double lat_offset = rad_to_deg(offset_north / EARTH_RADIUS);
    double lon_offset = rad_to_deg(offset_east / (EARTH_RADIUS * cos(lat_rad)));
    
    *out_lat = lat + lat_offset;
    *out_lon = lon + lon_offset;
}

// 设置真实位置（内部高精度）
bool location_set_precise(const PreciseLocation* location) {
    if (!location || !g_initialized) return false;
    
    // 验证坐标有效性
    if (!location_is_valid(location->latitude, location->longitude)) {
        return false;
    }
    
    pthread_mutex_lock(&g_location_mutex);
    
    // 安全复制位置数据
    memcpy(&g_precise_location, location, sizeof(PreciseLocation));
    g_precise_location.nonce = location_generate_nonce();
    
    pthread_mutex_unlock(&g_location_mutex);
    
    return true;
}

// 获取真实位置（仅内部使用，需要权限验证）
bool location_get_precise(PreciseLocation* location, const char* auth_token) {
    if (!location || !auth_token || !g_initialized) return false;
    
    // 验证权限令牌（简化实现，实际应使用更复杂的验证）
    // 这里仅作演示，实际应验证JWT或其他令牌
    if (strlen(auth_token) < 32) {
        return false;
    }
    
    pthread_mutex_lock(&g_location_mutex);
    
    memcpy(location, &g_precise_location, sizeof(PreciseLocation));
    
    pthread_mutex_unlock(&g_location_mutex);
    
    return true;
}

// 获取模糊位置（对外展示）
bool location_get_obfuscated(ObfuscatedLocation* location) {
    if (!location || !g_initialized) return false;
    
    pthread_mutex_lock(&g_location_mutex);
    
    // 模糊化坐标
    location_obfuscate_coordinates(g_precise_location.latitude, 
                                   g_precise_location.longitude,
                                   &location->latitude, 
                                   &location->longitude);
    
    // 设置模糊精度（500米左右）
    location->accuracy = OBFUSCATION_RADIUS_METERS + gaussian_random(0, 50);
    location->timestamp = time(NULL);
    
    // 生成模糊处理哈希（用于防破解验证）
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx) {
        unsigned char data[64];
        memcpy(data, &location->latitude, sizeof(double));
        memcpy(data + sizeof(double), &location->longitude, sizeof(double));
        memcpy(data + 2 * sizeof(double), &g_precise_location.nonce, sizeof(uint64_t));
        
        EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
        EVP_DigestUpdate(ctx, data, sizeof(double) * 2 + sizeof(uint64_t));
        EVP_DigestFinal_ex(ctx, location->obfuscation_hash, NULL);
        EVP_MD_CTX_free(ctx);
    }
    
    pthread_mutex_unlock(&g_location_mutex);
    
    return true;
}

// 反模糊化检测（检测是否尝试逆向）
bool location_detect_deobfuscation_attempt(const ObfuscatedLocation* location) {
    if (!location) return false;
    
    // 检测异常精度
    if (location->accuracy < OBFUSCATION_MIN_RADIUS) {
        return true; // 可能尝试逆向
    }
    
    // 检测哈希篡改
    if (!location_verify_integrity(location)) {
        return true;
    }
    
    return false;
}

// 验证位置数据完整性
bool location_verify_integrity(const ObfuscatedLocation* location) {
    if (!location) return false;
    
    // 验证坐标范围
    if (!location_is_valid(location->latitude, location->longitude)) {
        return false;
    }
    
    // 验证精度范围
    if (location->accuracy < OBFUSCATION_MIN_RADIUS || 
        location->accuracy > OBFUSCATION_MAX_RADIUS) {
        return false;
    }
    
    return true;
}

// 生成防重放随机数
uint64_t location_generate_nonce(void) {
    uint64_t nonce = 0;
    RAND_bytes((unsigned char*)&nonce, sizeof(nonce));
    return nonce;
}

// 计算两点距离（米）
double location_calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    double lat1_rad = deg_to_rad(lat1);
    double lat2_rad = deg_to_rad(lat2);
    double delta_lat = deg_to_rad(lat2 - lat1);
    double delta_lon = deg_to_rad(lon2 - lon1);
    
    double a = sin(delta_lat / 2) * sin(delta_lat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2) * sin(delta_lon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return EARTH_RADIUS * c;
}

// 检查位置是否在合理范围
bool location_is_valid(double lat, double lon) {
    return (lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0);
}

// 加密位置数据
bool location_encrypt(const PreciseLocation* plain, uint8_t* cipher, size_t* len) {
    if (!plain || !cipher || !len) return false;
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    
    int out_len = 0;
    int final_len = 0;
    
    // 使用AES-256-GCM加密
    uint8_t iv[12];
    RAND_bytes(iv, sizeof(iv));
    
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_EncryptInit_ex(ctx, NULL, NULL, g_device_key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 加密数据
    if (EVP_EncryptUpdate(ctx, cipher + sizeof(iv), &out_len, 
                          (const unsigned char*)plain, sizeof(PreciseLocation)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_EncryptFinal_ex(ctx, cipher + sizeof(iv) + out_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 获取认证标签
    uint8_t tag[16];
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, sizeof(tag), tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    // 复制IV到输出
    memcpy(cipher, iv, sizeof(iv));
    
    *len = sizeof(iv) + out_len + final_len + sizeof(tag);
    memcpy(cipher + sizeof(iv) + out_len + final_len, tag, sizeof(tag));
    
    return true;
}

// 解密位置数据
bool location_decrypt(const uint8_t* cipher, size_t len, PreciseLocation* plain) {
    if (!cipher || !plain || len < 28) return false; // 12(IV) + 16(tag) minimum
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    
    int out_len = 0;
    int final_len = 0;
    
    // 提取IV
    uint8_t iv[12];
    memcpy(iv, cipher, sizeof(iv));
    
    // 提取认证标签
    uint8_t tag[16];
    memcpy(tag, cipher + len - sizeof(tag), sizeof(tag));
    
    size_t cipher_len = len - sizeof(iv) - sizeof(tag);
    
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    if (EVP_DecryptInit_ex(ctx, NULL, NULL, g_device_key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 解密数据
    if (EVP_DecryptUpdate(ctx, (unsigned char*)plain, &out_len, 
                          cipher + sizeof(iv), cipher_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 设置认证标签
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, sizeof(tag), tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 验证并解密
    if (EVP_DecryptFinal_ex(ctx, (unsigned char*)plain + out_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false; // 认证失败
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    return true;
}

// 安全清除位置数据
void location_secure_clear(PreciseLocation* location) {
    if (!location) return;
    
    // 使用volatile防止编译器优化
    volatile unsigned char* p = (volatile unsigned char*)location;
    size_t size = sizeof(PreciseLocation);
    
    while (size--) {
        *p++ = 0;
    }
}

// 获取模糊区域边界
void location_get_obfuscation_bounds(double center_lat, double center_lon, 
                                     double* min_lat, double* max_lat,
                                     double* min_lon, double* max_lon) {
    if (!min_lat || !max_lat || !min_lon || !max_lon) return;
    
    double lat_rad = deg_to_rad(center_lat);
    
    // 计算边界偏移
    double lat_offset = rad_to_deg(OBFUSCATION_MAX_RADIUS / EARTH_RADIUS);
    double lon_offset = rad_to_deg(OBFUSCATION_MAX_RADIUS / (EARTH_RADIUS * cos(lat_rad)));
    
    *min_lat = center_lat - lat_offset;
    *max_lat = center_lat + lat_offset;
    *min_lon = center_lon - lon_offset;
    *max_lon = center_lon + lon_offset;
}
