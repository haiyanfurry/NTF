#ifndef ULTIMATE_PROTECTION_H
#define ULTIMATE_PROTECTION_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 保护级别
typedef enum {
    PROTECTION_LEVEL_LOW = 0,
    PROTECTION_LEVEL_MEDIUM,
    PROTECTION_LEVEL_HIGH,
    PROTECTION_LEVEL_ULTRA
} ProtectionLevel;

// 威胁类型
typedef enum {
    THREAT_NONE = 0,
    THREAT_DEBUGGER,
    THREAT_INJECTION,
    THREAT_TAMPERING,
    THREAT_VIRTUAL_MACHINE,
    THREAT_EMULATOR,
    THREAT_ROOT,
    THREAT_HOOK,
    THREAT_MEMORY_DUMP,
    THREAT_PACKET_CAPTURE,
    THREAT_REPACKAGING,
    THREAT_BRUTE_FORCE,
    THREAT_REPLAY_ATTACK,
    THREAT_MAN_IN_MIDDLE,
    THREAT_BLACKLIST_OS
} ThreatType;

// 初始化终极防护系统
bool ultimate_protection_init(ProtectionLevel level);

// 清理终极防护系统
void ultimate_protection_cleanup(void);

// 执行完整安全检查
ThreatType ultimate_protection_check_all(void);

// 防调试检测
bool ultimate_detect_debugger(void);

// 防注入检测
bool ultimate_detect_injection(void);

// 防篡改检测
bool ultimate_detect_tampering(void);

// 虚拟机检测
bool ultimate_detect_vm(void);

// 模拟器检测
bool ultimate_detect_emulator(void);

// Root/越狱检测
bool ultimate_detect_root(void);

// Hook检测
bool ultimate_detect_hook(void);

// 内存Dump检测
bool ultimate_detect_memory_dump(void);

// 抓包检测
bool ultimate_detect_packet_capture(void);

// 二次打包检测
bool ultimate_detect_repackaging(void);

// 暴力破解检测
bool ultimate_detect_brute_force(void);

// 重放攻击检测
bool ultimate_detect_replay_attack(void);

// 中间人攻击检测
bool ultimate_detect_mitm(void);

// 黑名单系统检测（Kali/Parrot等）
bool ultimate_detect_blacklist_os(void);

// 响应威胁
void ultimate_respond_to_threat(ThreatType threat);

// 启用自保护
bool ultimate_enable_self_protection(void);

// 禁用自保护
void ultimate_disable_self_protection(void);

// 设置威胁回调
void ultimate_set_threat_callback(void (*callback)(ThreatType));

// 获取最后检测到的威胁
ThreatType ultimate_get_last_threat(void);

// 获取威胁描述
const char* ultimate_get_threat_description(ThreatType threat);

#ifdef __cplusplus
}
#endif

#endif /* ULTIMATE_PROTECTION_H */
