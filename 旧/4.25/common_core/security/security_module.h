#ifndef SECURITY_MODULE_H
#define SECURITY_MODULE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BLOCKED_KEYWORDS 1000
#define MAX_SUSPICIOUS_USERS 500
#define MAX_AUDIT_LOG_ENTRIES 10000
#define MAX_RATE_LIMIT_ENTRIES 1000

typedef enum {
    SECURITY_LEVEL_NONE = 0,
    SECURITY_LEVEL_LOW,
    SECURITY_LEVEL_MEDIUM,
    SECURITY_LEVEL_HIGH,
    SECURITY_LEVEL_CRITICAL
} SecurityLevel;

typedef enum {
    SECURITY_EVENT_LOGIN = 1,
    SECURITY_EVENT_LOGOUT,
    SECURITY_EVENT_REGISTER,
    SECURITY_EVENT_MESSAGE_SENT,
    SECURITY_EVENT_MESSAGE_RECEIVED,
    SECURITY_EVENT_LOCATION_SHARED,
    SECURITY_EVENT_FRIEND_ADDED,
    SECURITY_EVENT_FRIEND_REMOVED,
    SECURITY_EVENT_MEDIA_UPLOADED,
    SECURITY_EVENT_MEDIA_DOWNLOADED,
    SECURITY_EVENT_GAME_JOINED,
    SECURITY_EVENT_GAME_ACTION,
    SECURITY_EVENT_SUSPICIOUS_ACTIVITY,
    SECURITY_EVENT_RATE_LIMIT_EXCEEDED,
    SECURITY_EVENT_CONTENT_VIOLATION,
    SECURITY_EVENT_PRIVACY_VIOLATION
} SecurityEventType;

typedef struct {
    uint64_t timestamp;
    SecurityEventType event_type;
    char uid[64];
    char ip[32];
    char details[512];
    SecurityLevel level;
} SecurityEvent;

typedef struct {
    char keyword[128];
    SecurityLevel level;
} BlockedKeyword;

typedef struct {
    char uid[64];
    uint64_t first_seen;
    uint64_t last_seen;
    int action_count;
    int message_count;
    int login_count;
    int location_updates;
    bool is_blocked;
    char reason[256];
} SuspiciousUser;

typedef struct {
    char uid[64];
    char action[64];
    uint64_t timestamp;
    int count;
    int window_seconds;
} RateLimitEntry;

typedef void (*SecurityAlertCallback)(SecurityEvent* event, void* user_data);
typedef void (*SecurityLogCallback)(SecurityEvent* event, void* user_data);

int security_module_init(void);
void security_module_destroy(void);

bool security_set_level(SecurityLevel level);
SecurityLevel security_get_level(void);

bool security_add_blocked_keyword(const char* keyword, SecurityLevel level);
bool security_remove_blocked_keyword(const char* keyword);
void security_clear_blocked_keywords(void);
int security_get_blocked_keyword_count(void);

bool check_message_content(const char* msg);
bool check_media_file(const char* path);
bool check_game_action(const char* uid, const char* game_action_json);
bool check_user_profile(const char* profile_json);

bool is_suspicious_user(const char* uid);
int get_suspicious_user_count(void);
SuspiciousUser* get_suspicious_user(int index);
bool block_user(const char* uid, const char* reason);
bool unblock_user(const char* uid);
void clear_suspicious_users(void);

bool check_rate_limit(const char* uid, const char* action, int window_seconds, int max_count);
bool set_rate_limit(const char* action, int window_seconds, int max_count);
void clear_rate_limits(void);

int report_user_action(const char* uid, const char* action_type, const char* details);
void log_security_event(SecurityEventType type, const char* uid, const char* details);
void log_security_event_with_ip(SecurityEventType type, const char* uid, const char* ip, const char* details);

void show_security_alert(const char* message);
void security_dump_events(void);
void security_dump_blocked_keywords(void);
void security_dump_suspicious_users(void);

void security_set_alert_callback(SecurityAlertCallback callback, void* user_data);
void security_set_log_callback(SecurityLogCallback callback, void* user_data);

const char* security_event_type_to_string(SecurityEventType type);
const char* security_level_to_string(SecurityLevel level);

int security_get_audit_log_count(void);
SecurityEvent* security_get_audit_log_entry(int index);
void security_clear_audit_log(void);
bool security_export_audit_log(const char* filename);

bool security_encrypt_data(const char* input, char* output, size_t output_size, const char* key);
bool security_decrypt_data(const char* input, char* output, size_t output_size, const char* key);

bool check_location_permission(const char* viewer_uid, const char* owner_uid);
double calculate_friend_distance(double lat1, double lon1, double lat2, double lon2);
const char* get_friend_distance_fuzzy(double distance_meters);

bool check_friend_relationship(const char* user1_uid, const char* user2_uid);
bool can_view_exact_location(const char* viewer_uid, const char* owner_uid);

#ifdef __cplusplus
}
#endif

#endif // SECURITY_MODULE_H
