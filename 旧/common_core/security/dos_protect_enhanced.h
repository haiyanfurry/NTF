#ifndef DOS_PROTECT_ENHANCED_H
#define DOS_PROTECT_ENHANCED_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// 配置常量
#define DOS_MAX_REQUESTS_PER_SECOND 100
#define DOS_MAX_REQUESTS_PER_MINUTE 1000
#define DOS_MAX_REQUESTS_PER_HOUR 10000
#define DOS_BLOCK_DURATION_SECONDS 3600  // 1小时
#define DOS_MAX_BLOCKED_IPS 1000
#define DOS_MAX_SUSPICIOUS_PATTERNS 50
#define DOS_RATE_LIMIT_WINDOW_MS 1000    // 1秒窗口

// IP地址结构
typedef struct {
    uint32_t ip;
    time_t first_request_time;
    time_t last_request_time;
    uint32_t request_count_second;
    uint32_t request_count_minute;
    uint32_t request_count_hour;
    uint32_t suspicious_score;
    bool is_blocked;
    time_t block_until;
    char reason[256];
} IPRateLimitEntry;

// 可疑行为模式
typedef enum {
    PATTERN_NORMAL = 0,
    PATTERN_BURST_REQUESTS,      // 突发请求
    PATTERN_SCANNING,            // 扫描行为
    PATTERN_BRUTE_FORCE,         // 暴力破解
    PATTERN_MALFORMED_PACKETS,   // 畸形包
    PATTERN_REPLAY_ATTACK,       // 重放攻击
    PATTERN_FLOOD_ATTACK,        // 洪水攻击
    PATTERN_SLOWLORIS,           // Slowloris攻击
    PATTERN_SYN_FLOOD,           // SYN Flood
    PATTERN_UDP_FLOOD            // UDP Flood
} SuspiciousPattern;

// 请求上下文
typedef struct {
    uint32_t client_ip;
    time_t timestamp;
    size_t packet_size;
    uint8_t packet_hash[32];     // 用于检测重放攻击
    SuspiciousPattern pattern;
} RequestContext;

// 初始化增强防DoS模块
bool dos_protect_enhanced_init(void);

// 清理增强防DoS模块
void dos_protect_enhanced_cleanup(void);

// 检查请求是否允许
bool dos_protect_check_request(const RequestContext* ctx);

// 记录请求
void dos_protect_record_request(const RequestContext* ctx);

// 拉黑IP
bool dos_protect_block_ip(uint32_t ip, const char* reason);

// 解封IP
bool dos_protect_unblock_ip(uint32_t ip);

// 检查IP是否被拉黑
bool dos_protect_is_ip_blocked(uint32_t ip);

// 获取IP的限流信息
const IPRateLimitEntry* dos_protect_get_ip_info(uint32_t ip);

// 检测可疑行为模式
SuspiciousPattern dos_protect_detect_pattern(const RequestContext* ctx);

// 增加可疑分数
void dos_protect_add_suspicious_score(uint32_t ip, int score, const char* reason);

// 获取系统负载
float dos_protect_get_system_load(void);

// 动态调整限流阈值
void dos_protect_adjust_thresholds(float system_load);

// 清理过期的限流记录
void dos_protect_cleanup_expired_entries(void);

// 获取被拉黑IP数量
int dos_protect_get_blocked_ip_count(void);

// 获取限流统计信息
void dos_protect_get_stats(uint32_t* total_requests, uint32_t* blocked_requests, 
                           uint32_t* active_ips, uint32_t* blocked_ips);

// 字符串IP转整数
uint32_t dos_protect_ip_string_to_int(const char* ip_str);

// 整数IP转字符串
void dos_protect_ip_int_to_string(uint32_t ip, char* buffer, size_t buffer_size);

// 生成请求哈希（用于重放攻击检测）
void dos_protect_generate_request_hash(const uint8_t* data, size_t len, uint8_t* hash);

// 检查是否为重放攻击
bool dos_protect_is_replay_attack(const uint8_t* hash);

#ifdef __cplusplus
}
#endif

#endif /* DOS_PROTECT_ENHANCED_H */
