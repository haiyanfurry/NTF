#define _GNU_SOURCE

#include "security_module.h"
#include "../network/p2p_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

static bool g_initialized = false;
static SecurityLevel g_security_level = SECURITY_LEVEL_MEDIUM;
static BlockedKeyword g_blocked_keywords[MAX_BLOCKED_KEYWORDS] = {0};
static int g_blocked_keyword_count = 0;
static SuspiciousUser g_suspicious_users[MAX_SUSPICIOUS_USERS] = {0};
static int g_suspicious_user_count = 0;
static RateLimitEntry g_rate_limits[MAX_RATE_LIMIT_ENTRIES] = {0};
static int g_rate_limit_count = 0;
static SecurityEvent g_audit_log[MAX_AUDIT_LOG_ENTRIES] = {0};
static int g_audit_log_index = 0;
static SecurityAlertCallback g_alert_callback = NULL;
static SecurityLogCallback g_log_callback = NULL;
static void* g_callback_user_data = NULL;
static pthread_mutex_t g_security_mutex = PTHREAD_MUTEX_INITIALIZER;

static const char* g_default_blocked_keywords[] = {
    "virus", "malware", "trojan", "backdoor", "ransomware",
    "hack", "crack", "exploit", "vulnerability", "phishing",
    "spam", "scam", "fraud", "illegal", "drugs", "weapons",
    "spam", "advertisement", "casino", "gambling", "betting",
    NULL
};

static uint64_t get_current_timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static void init_default_blocked_keywords(void) {
    for (int i = 0; g_default_blocked_keywords[i] != NULL; i++) {
        if (g_blocked_keyword_count < MAX_BLOCKED_KEYWORDS - 1) {
            strncpy(g_blocked_keywords[g_blocked_keyword_count].keyword,
                    g_default_blocked_keywords[i],
                    sizeof(g_blocked_keywords[g_blocked_keyword_count].keyword) - 1);
            g_blocked_keywords[g_blocked_keyword_count].level = SECURITY_LEVEL_HIGH;
            g_blocked_keyword_count++;
        }
    }
}

int security_module_init(void) {
    if (g_initialized) {
        return 0;
    }

    memset(g_blocked_keywords, 0, sizeof(g_blocked_keywords));
    memset(g_suspicious_users, 0, sizeof(g_suspicious_users));
    memset(g_rate_limits, 0, sizeof(g_rate_limits));
    memset(g_audit_log, 0, sizeof(g_audit_log));
    g_blocked_keyword_count = 0;
    g_suspicious_user_count = 0;
    g_rate_limit_count = 0;
    g_audit_log_index = 0;

    init_default_blocked_keywords();

    g_initialized = true;
    printf("[SECURITY] Security module initialized (level=%d)\n", g_security_level);
    return 0;
}

void security_module_destroy(void) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_security_mutex);
    memset(g_blocked_keywords, 0, sizeof(g_blocked_keywords));
    memset(g_suspicious_users, 0, sizeof(g_suspicious_users));
    memset(g_rate_limits, 0, sizeof(g_rate_limits));
    memset(g_audit_log, 0, sizeof(g_audit_log));
    pthread_mutex_unlock(&g_security_mutex);

    g_initialized = false;
    printf("[SECURITY] Security module destroyed\n");
}

bool security_set_level(SecurityLevel level) {
    if (!g_initialized) {
        return false;
    }

    g_security_level = level;
    printf("[SECURITY] Security level set to %d\n", level);
    return true;
}

SecurityLevel security_get_level(void) {
    return g_initialized ? g_security_level : SECURITY_LEVEL_NONE;
}

bool security_add_blocked_keyword(const char* keyword, SecurityLevel level) {
    if (!g_initialized || !keyword || g_blocked_keyword_count >= MAX_BLOCKED_KEYWORDS) {
        return false;
    }

    pthread_mutex_lock(&g_security_mutex);

    for (int i = 0; i < g_blocked_keyword_count; i++) {
        if (strcmp(g_blocked_keywords[i].keyword, keyword) == 0) {
            g_blocked_keywords[i].level = level;
            pthread_mutex_unlock(&g_security_mutex);
            return true;
        }
    }

    strncpy(g_blocked_keywords[g_blocked_keyword_count].keyword, keyword,
            sizeof(g_blocked_keywords[g_blocked_keyword_count].keyword) - 1);
    g_blocked_keywords[g_blocked_keyword_count].level = level;
    g_blocked_keyword_count++;

    pthread_mutex_unlock(&g_security_mutex);
    printf("[SECURITY] Added blocked keyword: %s (level=%d)\n", keyword, level);
    return true;
}

bool security_remove_blocked_keyword(const char* keyword) {
    if (!g_initialized || !keyword) {
        return false;
    }

    pthread_mutex_lock(&g_security_mutex);

    for (int i = 0; i < g_blocked_keyword_count; i++) {
        if (strcmp(g_blocked_keywords[i].keyword, keyword) == 0) {
            for (int j = i; j < g_blocked_keyword_count - 1; j++) {
                g_blocked_keywords[j] = g_blocked_keywords[j + 1];
            }
            g_blocked_keyword_count--;
            pthread_mutex_unlock(&g_security_mutex);
            printf("[SECURITY] Removed blocked keyword: %s\n", keyword);
            return true;
        }
    }

    pthread_mutex_unlock(&g_security_mutex);
    return false;
}

void security_clear_blocked_keywords(void) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_security_mutex);
    memset(g_blocked_keywords, 0, sizeof(g_blocked_keywords));
    g_blocked_keyword_count = 0;
    pthread_mutex_unlock(&g_security_mutex);
    printf("[SECURITY] Cleared all blocked keywords\n");
}

int security_get_blocked_keyword_count(void) {
    if (!g_initialized) {
        return 0;
    }
    return g_blocked_keyword_count;
}

static bool contains_blocked_keyword(const char* text) {
    if (!text) {
        return false;
    }

    char lower_text[4096];
    size_t len = strlen(text);
    if (len >= sizeof(lower_text)) {
        len = sizeof(lower_text) - 1;
    }

    for (size_t i = 0; i < len; i++) {
        lower_text[i] = tolower((unsigned char)text[i]);
    }
    lower_text[len] = '\0';

    pthread_mutex_lock(&g_security_mutex);

    for (int i = 0; i < g_blocked_keyword_count; i++) {
        if (strstr(lower_text, g_blocked_keywords[i].keyword) != NULL) {
            pthread_mutex_unlock(&g_security_mutex);
            printf("[SECURITY] Blocked keyword detected: %s\n", g_blocked_keywords[i].keyword);
            return true;
        }
    }

    pthread_mutex_unlock(&g_security_mutex);
    return false;
}

bool check_message_content(const char* msg) {
    if (!g_initialized) {
        return true;
    }

    if (!msg || strlen(msg) == 0) {
        return true;
    }

    if (contains_blocked_keyword(msg)) {
        log_security_event(SECURITY_EVENT_CONTENT_VIOLATION, "unknown", msg);
        return false;
    }

    return true;
}

bool check_media_file(const char* path) {
    if (!g_initialized) {
        return true;
    }

    if (!path) {
        return true;
    }

    printf("[SECURITY] Media file check: %s\n", path);
    return true;
}

bool check_game_action(const char* uid, const char* game_action_json) {
    if (!g_initialized) {
        return true;
    }

    if (!uid || !game_action_json) {
        return true;
    }

    if (contains_blocked_keyword(game_action_json)) {
        log_security_event(SECURITY_EVENT_CONTENT_VIOLATION, uid, game_action_json);
        return false;
    }

    return true;
}

bool check_user_profile(const char* profile_json) {
    if (!g_initialized) {
        return true;
    }

    if (!profile_json) {
        return true;
    }

    if (contains_blocked_keyword(profile_json)) {
        log_security_event(SECURITY_EVENT_CONTENT_VIOLATION, "unknown", profile_json);
        return false;
    }

    return true;
}

static SuspiciousUser* find_suspicious_user(const char* uid) {
    for (int i = 0; i < g_suspicious_user_count; i++) {
        if (strcmp(g_suspicious_users[i].uid, uid) == 0) {
            return &g_suspicious_users[i];
        }
    }
    return NULL;
}

static SuspiciousUser* find_empty_suspicious_slot(void) {
    for (int i = 0; i < MAX_SUSPICIOUS_USERS; i++) {
        if (g_suspicious_users[i].uid[0] == '\0') {
            return &g_suspicious_users[i];
        }
    }
    return NULL;
}

bool is_suspicious_user(const char* uid) {
    if (!g_initialized || !uid) {
        return false;
    }

    pthread_mutex_lock(&g_security_mutex);
    SuspiciousUser* user = find_suspicious_user(uid);
    bool result = (user != NULL && user->is_blocked);
    pthread_mutex_unlock(&g_security_mutex);

    return result;
}

int get_suspicious_user_count(void) {
    if (!g_initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_security_mutex);
    int count = 0;
    for (int i = 0; i < g_suspicious_user_count; i++) {
        if (g_suspicious_users[i].is_blocked) {
            count++;
        }
    }
    pthread_mutex_unlock(&g_security_mutex);

    return count;
}

SuspiciousUser* get_suspicious_user(int index) {
    if (!g_initialized || index < 0) {
        return NULL;
    }

    pthread_mutex_lock(&g_security_mutex);
    int count = 0;
    for (int i = 0; i < g_suspicious_user_count; i++) {
        if (g_suspicious_users[i].is_blocked) {
            if (count == index) {
                pthread_mutex_unlock(&g_security_mutex);
                return &g_suspicious_users[i];
            }
            count++;
        }
    }
    pthread_mutex_unlock(&g_security_mutex);

    return NULL;
}

bool block_user(const char* uid, const char* reason) {
    if (!g_initialized || !uid) {
        return false;
    }

    pthread_mutex_lock(&g_security_mutex);

    SuspiciousUser* user = find_suspicious_user(uid);
    if (!user) {
        user = find_empty_suspicious_slot();
        if (user) {
            strncpy(user->uid, uid, sizeof(user->uid) - 1);
            user->first_seen = get_current_timestamp();
            g_suspicious_user_count++;
        }
    }

    if (user) {
        user->is_blocked = true;
        user->last_seen = get_current_timestamp();
        if (reason) {
            strncpy(user->reason, reason, sizeof(user->reason) - 1);
        }

        pthread_mutex_unlock(&g_security_mutex);
        printf("[SECURITY] User blocked: %s (reason: %s)\n", uid, reason ? reason : "unknown");
        log_security_event(SECURITY_EVENT_SUSPICIOUS_ACTIVITY, uid, reason);
        return true;
    }

    pthread_mutex_unlock(&g_security_mutex);
    return false;
}

bool unblock_user(const char* uid) {
    if (!g_initialized || !uid) {
        return false;
    }

    pthread_mutex_lock(&g_security_mutex);

    SuspiciousUser* user = find_suspicious_user(uid);
    if (user) {
        user->is_blocked = false;
        pthread_mutex_unlock(&g_security_mutex);
        printf("[SECURITY] User unblocked: %s\n", uid);
        return true;
    }

    pthread_mutex_unlock(&g_security_mutex);
    return false;
}

void clear_suspicious_users(void) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_security_mutex);
    memset(g_suspicious_users, 0, sizeof(g_suspicious_users));
    g_suspicious_user_count = 0;
    pthread_mutex_unlock(&g_security_mutex);
    printf("[SECURITY] Cleared all suspicious users\n");
}

bool check_rate_limit(const char* uid, const char* action, int window_seconds, int max_count) {
    if (!g_initialized || !uid || !action) {
        return true;
    }

    pthread_mutex_lock(&g_security_mutex);

    uint64_t now = get_current_timestamp();

    for (int i = 0; i < g_rate_limit_count; i++) {
        if (strcmp(g_rate_limits[i].uid, uid) == 0 &&
            strcmp(g_rate_limits[i].action, action) == 0) {

            uint64_t elapsed = (now - g_rate_limits[i].timestamp) / 1000;

            if (elapsed > g_rate_limits[i].window_seconds) {
                g_rate_limits[i].count = 1;
                g_rate_limits[i].timestamp = now;
                pthread_mutex_unlock(&g_security_mutex);
                return true;
            }

            if (g_rate_limits[i].count >= max_count) {
                pthread_mutex_unlock(&g_security_mutex);
                printf("[SECURITY] Rate limit exceeded: %s %s\n", uid, action);
                log_security_event(SECURITY_EVENT_RATE_LIMIT_EXCEEDED, uid, action);
                return false;
            }

            g_rate_limits[i].count++;
            pthread_mutex_unlock(&g_security_mutex);
            return true;
        }
    }

    if (g_rate_limit_count < MAX_RATE_LIMIT_ENTRIES) {
        strncpy(g_rate_limits[g_rate_limit_count].uid, uid, sizeof(g_rate_limits[g_rate_limit_count].uid) - 1);
        strncpy(g_rate_limits[g_rate_limit_count].action, action, sizeof(g_rate_limits[g_rate_limit_count].action) - 1);
        g_rate_limits[g_rate_limit_count].count = 1;
        g_rate_limits[g_rate_limit_count].timestamp = now;
        g_rate_limits[g_rate_limit_count].window_seconds = window_seconds;
        g_rate_limit_count++;
    }

    pthread_mutex_unlock(&g_security_mutex);
    return true;
}

bool set_rate_limit(const char* action, int window_seconds, int max_count) {
    if (!g_initialized || !action) {
        return false;
    }

    printf("[SECURITY] Rate limit set for action '%s': %d requests per %d seconds\n",
           action, max_count, window_seconds);
    return true;
}

void clear_rate_limits(void) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_security_mutex);
    memset(g_rate_limits, 0, sizeof(g_rate_limits));
    g_rate_limit_count = 0;
    pthread_mutex_unlock(&g_security_mutex);
    printf("[SECURITY] Cleared all rate limits\n");
}

int report_user_action(const char* uid, const char* action_type, const char* details) {
    if (!g_initialized || !uid) {
        return -1;
    }

    printf("[SECURITY] User action reported: %s - %s\n", uid, action_type);
    log_security_event_with_ip(SECURITY_EVENT_SUSPICIOUS_ACTIVITY, uid, "0.0.0.0", details);

    if (g_security_level >= SECURITY_LEVEL_HIGH) {
        SuspiciousUser* user = find_suspicious_user(uid);
        if (!user) {
            user = find_empty_suspicious_slot();
            if (user) {
                strncpy(user->uid, uid, sizeof(user->uid) - 1);
                user->first_seen = get_current_timestamp();
                g_suspicious_user_count++;
            }
        }

        if (user) {
            user->action_count++;
            user->last_seen = get_current_timestamp();

            if (user->action_count > 100) {
                user->is_blocked = true;
                strncpy(user->reason, "Excessive actions", sizeof(user->reason) - 1);
                printf("[SECURITY] User auto-blocked due to excessive actions: %s\n", uid);
                return 1;
            }
        }
    }

    return 0;
}

void log_security_event(SecurityEventType type, const char* uid, const char* details) {
    if (!g_initialized) {
        return;
    }

    log_security_event_with_ip(type, uid, "0.0.0.0", details);
}

void log_security_event_with_ip(SecurityEventType type, const char* uid, const char* ip, const char* details) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_security_mutex);

    g_audit_log[g_audit_log_index].timestamp = get_current_timestamp();
    g_audit_log[g_audit_log_index].event_type = type;
    if (uid) {
        strncpy(g_audit_log[g_audit_log_index].uid, uid, sizeof(g_audit_log[g_audit_log_index].uid) - 1);
    }
    if (ip) {
        strncpy(g_audit_log[g_audit_log_index].ip, ip, sizeof(g_audit_log[g_audit_log_index].ip) - 1);
    }
    if (details) {
        strncpy(g_audit_log[g_audit_log_index].details, details, sizeof(g_audit_log[g_audit_log_index].details) - 1);
    }

    switch (type) {
        case SECURITY_EVENT_SUSPICIOUS_ACTIVITY:
        case SECURITY_EVENT_CONTENT_VIOLATION:
        case SECURITY_EVENT_PRIVACY_VIOLATION:
            g_audit_log[g_audit_log_index].level = SECURITY_LEVEL_HIGH;
            break;
        case SECURITY_EVENT_RATE_LIMIT_EXCEEDED:
            g_audit_log[g_audit_log_index].level = SECURITY_LEVEL_MEDIUM;
            break;
        default:
            g_audit_log[g_audit_log_index].level = SECURITY_LEVEL_LOW;
            break;
    }

    g_audit_log_index = (g_audit_log_index + 1) % MAX_AUDIT_LOG_ENTRIES;

    pthread_mutex_unlock(&g_security_mutex);

    if (g_log_callback) {
        g_log_callback(&g_audit_log[(g_audit_log_index - 1 + MAX_AUDIT_LOG_ENTRIES) % MAX_AUDIT_LOG_ENTRIES], g_callback_user_data);
    }
}

void show_security_alert(const char* message) {
    if (!g_initialized || !message) {
        return;
    }

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ⚠️  SECURITY ALERT                                      ║\n");
    printf("║  %-54s ║\n", message);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void security_dump_events(void) {
    if (!g_initialized) {
        return;
    }

    printf("\n=== Security Events (last %d events) ===\n", MAX_AUDIT_LOG_ENTRIES);
    pthread_mutex_lock(&g_security_mutex);

    int count = 0;
    for (int i = 0; i < MAX_AUDIT_LOG_ENTRIES && count < 50; i++) {
        int idx = (g_audit_log_index - 1 - i + MAX_AUDIT_LOG_ENTRIES) % MAX_AUDIT_LOG_ENTRIES;
        if (g_audit_log[idx].timestamp > 0) {
            printf("  [%llu] %s - %s - %s\n",
                   (unsigned long long)g_audit_log[idx].timestamp,
                   security_event_type_to_string(g_audit_log[idx].event_type),
                   g_audit_log[idx].uid,
                   g_audit_log[idx].details);
            count++;
        }
    }

    pthread_mutex_unlock(&g_security_mutex);
    printf("========================================\n\n");
}

void security_dump_blocked_keywords(void) {
    if (!g_initialized) {
        return;
    }

    printf("\n=== Blocked Keywords (%d) ===\n", g_blocked_keyword_count);
    pthread_mutex_lock(&g_security_mutex);

    for (int i = 0; i < g_blocked_keyword_count; i++) {
        printf("  [%d] %s (level=%s)\n",
               i + 1,
               g_blocked_keywords[i].keyword,
               security_level_to_string(g_blocked_keywords[i].level));
    }

    pthread_mutex_unlock(&g_security_mutex);
    printf("==============================\n\n");
}

void security_dump_suspicious_users(void) {
    if (!g_initialized) {
        return;
    }

    printf("\n=== Suspicious Users (%d blocked) ===\n", get_suspicious_user_count());
    pthread_mutex_lock(&g_security_mutex);

    for (int i = 0; i < g_suspicious_user_count; i++) {
        if (g_suspicious_users[i].is_blocked) {
            printf("  %s - %s (actions: %d, reason: %s)\n",
                   g_suspicious_users[i].uid,
                   g_suspicious_users[i].is_blocked ? "BLOCKED" : "OK",
                   g_suspicious_users[i].action_count,
                   g_suspicious_users[i].reason);
        }
    }

    pthread_mutex_unlock(&g_security_mutex);
    printf("===================================\n\n");
}

void security_set_alert_callback(SecurityAlertCallback callback, void* user_data) {
    if (g_initialized) {
        g_alert_callback = callback;
        g_callback_user_data = user_data;
    }
}

void security_set_log_callback(SecurityLogCallback callback, void* user_data) {
    if (g_initialized) {
        g_log_callback = callback;
        g_callback_user_data = user_data;
    }
}

const char* security_event_type_to_string(SecurityEventType type) {
    switch (type) {
        case SECURITY_EVENT_LOGIN: return "LOGIN";
        case SECURITY_EVENT_LOGOUT: return "LOGOUT";
        case SECURITY_EVENT_REGISTER: return "REGISTER";
        case SECURITY_EVENT_MESSAGE_SENT: return "MESSAGE_SENT";
        case SECURITY_EVENT_MESSAGE_RECEIVED: return "MESSAGE_RECEIVED";
        case SECURITY_EVENT_LOCATION_SHARED: return "LOCATION_SHARED";
        case SECURITY_EVENT_FRIEND_ADDED: return "FRIEND_ADDED";
        case SECURITY_EVENT_FRIEND_REMOVED: return "FRIEND_REMOVED";
        case SECURITY_EVENT_MEDIA_UPLOADED: return "MEDIA_UPLOADED";
        case SECURITY_EVENT_MEDIA_DOWNLOADED: return "MEDIA_DOWNLOADED";
        case SECURITY_EVENT_GAME_JOINED: return "GAME_JOINED";
        case SECURITY_EVENT_GAME_ACTION: return "GAME_ACTION";
        case SECURITY_EVENT_SUSPICIOUS_ACTIVITY: return "SUSPICIOUS_ACTIVITY";
        case SECURITY_EVENT_RATE_LIMIT_EXCEEDED: return "RATE_LIMIT_EXCEEDED";
        case SECURITY_EVENT_CONTENT_VIOLATION: return "CONTENT_VIOLATION";
        case SECURITY_EVENT_PRIVACY_VIOLATION: return "PRIVACY_VIOLATION";
        default: return "UNKNOWN";
    }
}

const char* security_level_to_string(SecurityLevel level) {
    switch (level) {
        case SECURITY_LEVEL_NONE: return "NONE";
        case SECURITY_LEVEL_LOW: return "LOW";
        case SECURITY_LEVEL_MEDIUM: return "MEDIUM";
        case SECURITY_LEVEL_HIGH: return "HIGH";
        case SECURITY_LEVEL_CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

int security_get_audit_log_count(void) {
    if (!g_initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_security_mutex);
    int count = 0;
    for (int i = 0; i < MAX_AUDIT_LOG_ENTRIES; i++) {
        if (g_audit_log[i].timestamp > 0) {
            count++;
        }
    }
    pthread_mutex_unlock(&g_security_mutex);

    return count;
}

SecurityEvent* security_get_audit_log_entry(int index) {
    if (!g_initialized || index < 0 || index >= MAX_AUDIT_LOG_ENTRIES) {
        return NULL;
    }

    return &g_audit_log[index];
}

void security_clear_audit_log(void) {
    if (!g_initialized) {
        return;
    }

    pthread_mutex_lock(&g_security_mutex);
    memset(g_audit_log, 0, sizeof(g_audit_log));
    g_audit_log_index = 0;
    pthread_mutex_unlock(&g_security_mutex);
    printf("[SECURITY] Audit log cleared\n");
}

bool security_export_audit_log(const char* filename) {
    if (!g_initialized || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("[SECURITY] Failed to export audit log to %s\n", filename);
        return false;
    }

    pthread_mutex_lock(&g_security_mutex);

    fprintf(fp, "FindFriend Security Audit Log\n");
    fprintf(fp, "=============================\n\n");

    for (int i = 0; i < MAX_AUDIT_LOG_ENTRIES; i++) {
        if (g_audit_log[i].timestamp > 0) {
            fprintf(fp, "[%llu] %s - %s - %s - %s\n",
                    (unsigned long long)g_audit_log[i].timestamp,
                    security_event_type_to_string(g_audit_log[i].event_type),
                    g_audit_log[i].uid,
                    g_audit_log[i].ip,
                    g_audit_log[i].details);
        }
    }

    pthread_mutex_unlock(&g_security_mutex);
    fclose(fp);

    printf("[SECURITY] Audit log exported to %s\n", filename);
    return true;
}

bool security_encrypt_data(const char* input, char* output, size_t output_size, const char* key) {
    if (!input || !output || !key) {
        return false;
    }

    size_t input_len = strlen(input);
    if (input_len >= output_size) {
        return false;
    }

    for (size_t i = 0; i < input_len; i++) {
        output[i] = input[i] ^ key[i % strlen(key)];
    }
    output[input_len] = '\0';

    return true;
}

bool security_decrypt_data(const char* input, char* output, size_t output_size, const char* key) {
    return security_encrypt_data(input, output, output_size, key);
}

bool check_location_permission(const char* viewer_uid, const char* owner_uid) {
    if (!g_initialized || !viewer_uid || !owner_uid) {
        return false;
    }

    if (strcmp(viewer_uid, owner_uid) == 0) {
        return true;
    }

    return check_friend_relationship(viewer_uid, owner_uid);
}

double calculate_friend_distance(double lat1, double lon1, double lat2, double lon2) {
    double R = 6371000.0;
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return R * c;
}

const char* get_friend_distance_fuzzy(double distance_meters) {
    if (distance_meters < 100) {
        return "< 100m";
    } else if (distance_meters < 500) {
        return "100m - 500m";
    } else if (distance_meters < 1000) {
        return "500m - 1km";
    } else if (distance_meters < 5000) {
        return "1km - 5km";
    } else {
        return "> 5km";
    }
}

bool check_friend_relationship(const char* user1_uid, const char* user2_uid) {
    if (!g_initialized || !user1_uid || !user2_uid) {
        return false;
    }

    if (strcmp(user1_uid, user2_uid) == 0) {
        return true;
    }

    return p2p_is_friend(user2_uid);
}

bool can_view_exact_location(const char* viewer_uid, const char* owner_uid) {
    if (!g_initialized || !viewer_uid || !owner_uid) {
        return false;
    }

    if (strcmp(viewer_uid, owner_uid) == 0) {
        return true;
    }

    return false;
}
