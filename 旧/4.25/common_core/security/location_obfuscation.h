#ifndef LOCATION_OBFUSCATION_H
#define LOCATION_OBFUSCATION_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// 模糊半径配置（米）
#define OBFUSCATION_RADIUS_METERS 500.0
#define OBFUSCATION_MIN_RADIUS 300.0
#define OBFUSCATION_MAX_RADIUS 700.0

// 内部高精度配置（厘米）
#define INTERNAL_PRECISION_CM 1.0

// 坐标结构体（高精度）
typedef struct {
    double latitude;   // 纬度（度）
    double longitude;  // 经度（度）
    double altitude;   // 海拔（米）
    double accuracy;   // 精度（米）
    time_t timestamp;  // 时间戳
    uint64_t nonce;    // 随机数（防重放）
} PreciseLocation;

// 模糊坐标结构体（对外展示）
typedef struct {
    double latitude;   // 模糊纬度
    double longitude;  // 模糊经度
    double accuracy;   // 模糊精度（约500米）
    time_t timestamp;  // 时间戳
    uint8_t obfuscation_hash[32];  // 模糊处理哈希（防破解）
} ObfuscatedLocation;

// 初始化模糊定位模块
bool location_obfuscation_init(void);

// 清理模糊定位模块
void location_obfuscation_cleanup(void);

// 设置真实位置（内部高精度）
bool location_set_precise(const PreciseLocation* location);

// 获取真实位置（仅内部使用，需要权限验证）
bool location_get_precise(PreciseLocation* location, const char* auth_token);

// 获取模糊位置（对外展示）
bool location_get_obfuscated(ObfuscatedLocation* location);

// 模糊化坐标（核心算法）
void location_obfuscate_coordinates(double lat, double lon, double* out_lat, double* out_lon);

// 反模糊化检测（检测是否尝试逆向）
bool location_detect_deobfuscation_attempt(const ObfuscatedLocation* location);

// 验证位置数据完整性
bool location_verify_integrity(const ObfuscatedLocation* location);

// 生成防重放随机数
uint64_t location_generate_nonce(void);

// 计算两点距离（米）
double location_calculate_distance(double lat1, double lon1, double lat2, double lon2);

// 检查位置是否在合理范围
bool location_is_valid(double lat, double lon);

// 加密位置数据
bool location_encrypt(const PreciseLocation* plain, uint8_t* cipher, size_t* len);

// 解密位置数据
bool location_decrypt(const uint8_t* cipher, size_t len, PreciseLocation* plain);

// 安全清除位置数据
void location_secure_clear(PreciseLocation* location);

// 获取模糊区域边界
void location_get_obfuscation_bounds(double center_lat, double center_lon, 
                                     double* min_lat, double* max_lat,
                                     double* min_lon, double* max_lon);

#ifdef __cplusplus
}
#endif

#endif /* LOCATION_OBFUSCATION_H */
