#include "dos_protect_enhanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <openssl/evp.h>

// 全局状态
static IPRateLimitEntry g_ip_entries[DOS_MAX_BLOCKED_IPS];
static pthread_mutex_t g_dos_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint32_t g_total_requests = 0;
static uint32_t g_blocked_requests = 0;
static time_t g_last_cleanup = 0;

// 重放攻击检测缓存
#define REPLAY_CACHE_SIZE 1000
static uint8_t g_replay_cache[REPLAY_CACHE_SIZE][32];
static int g_replay_cache_index = 0;

// 初始化增强防DoS模块
bool dos_protect_enhanced_init(void) {
    pthread_mutex_lock(&g_dos_mutex);
    memset(g_ip_entries, 0, sizeof(g_ip_entries));
    memset(g_replay_cache, 0, sizeof(g_replay_cache));
    g_total_requests = 0;
    g_blocked_requests = 0;
    g_last_cleanup = time(NULL);
    pthread_mutex_unlock(&g_dos_mutex);
    
    printf("[DoS] Enhanced protection initialized\n");
    return true;
}

// 清理增强防DoS模块
void dos_protect_enhanced_cleanup(void) {
    pthread_mutex_lock(&g_dos_mutex);
    memset(g_ip_entries, 0, sizeof(g_ip_entries));
    memset(g_replay_cache, 0, sizeof(g_replay_cache));
    pthread_mutex_unlock(&g_dos_mutex);
    
    printf("[DoS] Enhanced protection cleaned up\n");
}

// 查找或创建IP条目
static IPRateLimitEntry* find_or_create_ip_entry(uint32_t ip) {
    time_t now = time(NULL);
    IPRateLimitEntry* oldest = NULL;
    time_t oldest_time = now;
    
    for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
        if (g_ip_entries[i].ip == ip) {
            return &g_ip_entries[i];
        }
        
        // 记录最老的条目用于替换
        if (g_ip_entries[i].first_request_time < oldest_time) {
            oldest_time = g_ip_entries[i].first_request_time;
            oldest = &g_ip_entries[i];
        }
    }
    
    // 使用最老的条目或空条目
    if (oldest) {
        memset(oldest, 0, sizeof(IPRateLimitEntry));
        oldest->ip = ip;
        oldest->first_request_time = now;
        oldest->last_request_time = now;
        return oldest;
    }
    
    return NULL;
}

// 检查请求是否允许
bool dos_protect_check_request(const RequestContext* ctx) {
    if (!ctx) return false;
    
    pthread_mutex_lock(&g_dos_mutex);
    g_total_requests++;
    
    // 查找IP条目
    IPRateLimitEntry* entry = find_or_create_ip_entry(ctx->client_ip);
    if (!entry) {
        pthread_mutex_unlock(&g_dos_mutex);
        g_blocked_requests++;
        return false; // 无法创建条目，拒绝请求
    }
    
    time_t now = time(NULL);
    
    // 检查是否被拉黑
    if (entry->is_blocked) {
        if (now < entry->block_until) {
            // 仍在封禁期
            pthread_mutex_unlock(&g_dos_mutex);
            g_blocked_requests++;
            return false;
        } else {
            // 解封
            entry->is_blocked = false;
            entry->suspicious_score = 0;
            memset(entry->reason, 0, sizeof(entry->reason));
        }
    }
    
    // 重置计数器（时间窗口）
    if (now - entry->last_request_time >= 1) {
        entry->request_count_second = 0;
    }
    if (now - entry->first_request_time >= 60) {
        entry->request_count_minute = 0;
        entry->first_request_time = now;
    }
    if (now - entry->first_request_time >= 3600) {
        entry->request_count_hour = 0;
    }
    
    // 检查限流
    if (entry->request_count_second >= DOS_MAX_REQUESTS_PER_SECOND ||
        entry->request_count_minute >= DOS_MAX_REQUESTS_PER_MINUTE ||
        entry->request_count_hour >= DOS_MAX_REQUESTS_PER_HOUR) {
        
        entry->is_blocked = true;
        entry->block_until = now + DOS_BLOCK_DURATION_SECONDS;
        strncpy(entry->reason, "Rate limit exceeded", sizeof(entry->reason) - 1);
        
        printf("[DoS] IP %u blocked: Rate limit exceeded\n", ctx->client_ip);
        pthread_mutex_unlock(&g_dos_mutex);
        g_blocked_requests++;
        return false;
    }
    
    // 检查可疑分数
    if (entry->suspicious_score >= 100) {
        entry->is_blocked = true;
        entry->block_until = now + DOS_BLOCK_DURATION_SECONDS;
        strncpy(entry->reason, "Suspicious behavior detected", sizeof(entry->reason) - 1);
        
        printf("[DoS] IP %u blocked: Suspicious behavior (score: %u)\n", 
               ctx->client_ip, entry->suspicious_score);
        pthread_mutex_unlock(&g_dos_mutex);
        g_blocked_requests++;
        return false;
    }
    
    // 检查重放攻击
    if (dos_protect_is_replay_attack(ctx->packet_hash)) {
        entry->suspicious_score += 50;
        printf("[DoS] Replay attack detected from IP %u\n", ctx->client_ip);
        pthread_mutex_unlock(&g_dos_mutex);
        g_blocked_requests++;
        return false;
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
    return true;
}

// 记录请求
void dos_protect_record_request(const RequestContext* ctx) {
    if (!ctx) return;
    
    pthread_mutex_lock(&g_dos_mutex);
    
    IPRateLimitEntry* entry = find_or_create_ip_entry(ctx->client_ip);
    if (entry) {
        time_t now = time(NULL);
        entry->last_request_time = now;
        entry->request_count_second++;
        entry->request_count_minute++;
        entry->request_count_hour++;
        
        // 根据行为模式增加可疑分数
        switch (ctx->pattern) {
            case PATTERN_BURST_REQUESTS:
                entry->suspicious_score += 10;
                break;
            case PATTERN_SCANNING:
                entry->suspicious_score += 30;
                break;
            case PATTERN_BRUTE_FORCE:
                entry->suspicious_score += 50;
                break;
            case PATTERN_MALFORMED_PACKETS:
                entry->suspicious_score += 40;
                break;
            case PATTERN_REPLAY_ATTACK:
                entry->suspicious_score += 50;
                break;
            case PATTERN_FLOOD_ATTACK:
                entry->suspicious_score += 60;
                break;
            default:
                break;
        }
    }
    
    // 存储请求哈希用于重放检测
    memcpy(g_replay_cache[g_replay_cache_index], ctx->packet_hash, 32);
    g_replay_cache_index = (g_replay_cache_index + 1) % REPLAY_CACHE_SIZE;
    
    pthread_mutex_unlock(&g_dos_mutex);
}

// 拉黑IP
bool dos_protect_block_ip(uint32_t ip, const char* reason) {
    pthread_mutex_lock(&g_dos_mutex);
    
    IPRateLimitEntry* entry = find_or_create_ip_entry(ip);
    if (entry) {
        entry->is_blocked = true;
        entry->block_until = time(NULL) + DOS_BLOCK_DURATION_SECONDS;
        if (reason) {
            strncpy(entry->reason, reason, sizeof(entry->reason) - 1);
        }
        printf("[DoS] IP %u manually blocked: %s\n", ip, reason ? reason : "No reason");
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
    return entry != NULL;
}

// 解封IP
bool dos_protect_unblock_ip(uint32_t ip) {
    pthread_mutex_lock(&g_dos_mutex);
    
    for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
        if (g_ip_entries[i].ip == ip) {
            g_ip_entries[i].is_blocked = false;
            g_ip_entries[i].suspicious_score = 0;
            g_ip_entries[i].block_until = 0;
            memset(g_ip_entries[i].reason, 0, sizeof(g_ip_entries[i].reason));
            printf("[DoS] IP %u unblocked\n", ip);
            pthread_mutex_unlock(&g_dos_mutex);
            return true;
        }
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
    return false;
}

// 检查IP是否被拉黑
bool dos_protect_is_ip_blocked(uint32_t ip) {
    pthread_mutex_lock(&g_dos_mutex);
    
    for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
        if (g_ip_entries[i].ip == ip && g_ip_entries[i].is_blocked) {
            time_t now = time(NULL);
            if (now < g_ip_entries[i].block_until) {
                pthread_mutex_unlock(&g_dos_mutex);
                return true;
            } else {
                // 解封过期IP
                g_ip_entries[i].is_blocked = false;
            }
        }
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
    return false;
}

// 获取IP的限流信息
const IPRateLimitEntry* dos_protect_get_ip_info(uint32_t ip) {
    pthread_mutex_lock(&g_dos_mutex);
    
    for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
        if (g_ip_entries[i].ip == ip) {
            pthread_mutex_unlock(&g_dos_mutex);
            return &g_ip_entries[i];
        }
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
    return NULL;
}

// 检测可疑行为模式
SuspiciousPattern dos_protect_detect_pattern(const RequestContext* ctx) {
    if (!ctx) return PATTERN_NORMAL;
    
    // 检查包大小异常
    if (ctx->packet_size > 65535) {
        return PATTERN_MALFORMED_PACKETS;
    }
    
    // 检查包大小过小（可能的攻击探测）
    if (ctx->packet_size < 10) {
        return PATTERN_SCANNING;
    }
    
    return ctx->pattern;
}

// 增加可疑分数
void dos_protect_add_suspicious_score(uint32_t ip, int score, const char* reason) {
    pthread_mutex_lock(&g_dos_mutex);
    
    IPRateLimitEntry* entry = find_or_create_ip_entry(ip);
    if (entry) {
        entry->suspicious_score += score;
        printf("[DoS] IP %u suspicious score increased by %d (total: %u): %s\n",
               ip, score, entry->suspicious_score, reason ? reason : "No reason");
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
}

// 获取系统负载
float dos_protect_get_system_load(void) {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return (float)info.loads[0] / (1 << SI_LOAD_SHIFT);
    }
    return 0.0f;
}

// 动态调整限流阈值
void dos_protect_adjust_thresholds(float system_load) {
    // 根据系统负载动态调整
    // 高负载时降低阈值
    if (system_load > 2.0f) {
        printf("[DoS] High system load detected (%.2f), adjusting thresholds\n", system_load);
    }
}

// 清理过期的限流记录
void dos_protect_cleanup_expired_entries(void) {
    time_t now = time(NULL);
    
    // 每5分钟清理一次
    if (now - g_last_cleanup < 300) {
        return;
    }
    
    pthread_mutex_lock(&g_dos_mutex);
    
    for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
        if (g_ip_entries[i].ip != 0) {
            // 清理超过1小时无活动的条目
            if (now - g_ip_entries[i].last_request_time > 3600 && !g_ip_entries[i].is_blocked) {
                memset(&g_ip_entries[i], 0, sizeof(IPRateLimitEntry));
            }
            // 清理已过期的封禁
            else if (g_ip_entries[i].is_blocked && now > g_ip_entries[i].block_until) {
                g_ip_entries[i].is_blocked = false;
                g_ip_entries[i].suspicious_score = 0;
            }
        }
    }
    
    g_last_cleanup = now;
    pthread_mutex_unlock(&g_dos_mutex);
}

// 获取被拉黑IP数量
int dos_protect_get_blocked_ip_count(void) {
    int count = 0;
    
    pthread_mutex_lock(&g_dos_mutex);
    
    for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
        if (g_ip_entries[i].is_blocked) {
            count++;
        }
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
    return count;
}

// 获取限流统计信息
void dos_protect_get_stats(uint32_t* total_requests, uint32_t* blocked_requests,
                           uint32_t* active_ips, uint32_t* blocked_ips) {
    pthread_mutex_lock(&g_dos_mutex);
    
    if (total_requests) *total_requests = g_total_requests;
    if (blocked_requests) *blocked_requests = g_blocked_requests;
    
    if (active_ips) {
        *active_ips = 0;
        for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
            if (g_ip_entries[i].ip != 0) {
                (*active_ips)++;
            }
        }
    }
    
    if (blocked_ips) {
        *blocked_ips = 0;
        for (int i = 0; i < DOS_MAX_BLOCKED_IPS; i++) {
            if (g_ip_entries[i].is_blocked) {
                (*blocked_ips)++;
            }
        }
    }
    
    pthread_mutex_unlock(&g_dos_mutex);
}

// 字符串IP转整数
uint32_t dos_protect_ip_string_to_int(const char* ip_str) {
    if (!ip_str) return 0;
    
    unsigned int a, b, c, d;
    if (sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
        return (a << 24) | (b << 16) | (c << 8) | d;
    }
    return 0;
}

// 整数IP转字符串
void dos_protect_ip_int_to_string(uint32_t ip, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < 16) return;
    
    snprintf(buffer, buffer_size, "%u.%u.%u.%u",
             (ip >> 24) & 0xFF,
             (ip >> 16) & 0xFF,
             (ip >> 8) & 0xFF,
             ip & 0xFF);
}

// 生成请求哈希（用于重放攻击检测）
void dos_protect_generate_request_hash(const uint8_t* data, size_t len, uint8_t* hash) {
    if (!data || !hash || len == 0) return;
    
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx) {
        EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
        EVP_DigestUpdate(ctx, data, len);
        EVP_DigestFinal_ex(ctx, hash, NULL);
        EVP_MD_CTX_free(ctx);
    }
}

// 检查是否为重放攻击
bool dos_protect_is_replay_attack(const uint8_t* hash) {
    if (!hash) return false;
    
    for (int i = 0; i < REPLAY_CACHE_SIZE; i++) {
        if (memcmp(g_replay_cache[i], hash, 32) == 0) {
            return true; // 发现重放攻击
        }
    }
    return false;
}
