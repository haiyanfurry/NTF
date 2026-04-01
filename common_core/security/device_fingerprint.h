#ifndef DEVICE_FINGERPRINT_H
#define DEVICE_FINGERPRINT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 设备指纹结构体
typedef struct {
    char fingerprint[256];      // 设备指纹
    char device_info[512];      // 设备信息
    bool is_emulator;           // 是否为模拟器
    bool is_virtual_machine;    // 是否为虚拟机
    int risk_score;             // 风险评分
} DeviceFingerprint;

// 初始化设备指纹模块
bool device_fingerprint_init(void);

// 生成设备指纹
DeviceFingerprint* device_fingerprint_generate(void);

// 验证设备指纹
bool device_fingerprint_validate(const char* fingerprint);

// 检测模拟器
bool device_fingerprint_is_emulator(void);

// 检测虚拟机
bool device_fingerprint_is_virtual_machine(void);

// 计算风险评分
int device_fingerprint_calculate_risk(void);

// 销毁设备指纹
void device_fingerprint_destroy(DeviceFingerprint* fingerprint);

#ifdef __cplusplus
}
#endif

#endif // DEVICE_FINGERPRINT_H