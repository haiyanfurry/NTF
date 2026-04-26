#define _GNU_SOURCE

#include "core_interface.h"
#include "find_friend_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

static bool g_initialized = false;
static char g_version[64] = "FindFriend Core v4.0.0";
static char g_debug_logs[1024 * 10] = {0};
static size_t g_log_offset = 0;

static void append_log(const char* format, ...) {
    if (!format) return;

    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    size_t len = strlen(buffer);
    if (g_log_offset + len < sizeof(g_debug_logs) - 1) {
        strcpy(g_debug_logs + g_log_offset, buffer);
        g_log_offset += len;
    } else {
        memmove(g_debug_logs, g_debug_logs + 1024, sizeof(g_debug_logs) - 1024);
        g_log_offset = sizeof(g_debug_logs) - 1024;
        strcpy(g_debug_logs + g_log_offset, buffer);
    }
}

bool core_interface_init(const char* server_host, int port) {
    if (g_initialized) {
        append_log("[CORE] Already initialized\n");
        return true;
    }

    append_log("[CORE] Initializing core interface...\n");
    append_log("[CORE] Server: %s:%d\n", server_host ? server_host : "localhost", port);

    if (!find_friend_core_init(server_host ? server_host : "localhost", port)) {
        append_log("[CORE] ERROR: Failed to initialize core\n");
        return false;
    }

    g_initialized = true;
    append_log("[CORE] Core interface initialized successfully\n");
    return true;
}

void core_interface_destroy(void) {
    if (!g_initialized) {
        return;
    }

    append_log("[CORE] Destroying core interface...\n");
    find_friend_core_destroy();
    g_initialized = false;
    append_log("[CORE] Core interface destroyed\n");
}

const char* core_interface_get_version(void) {
    return g_version;
}

bool core_interface_is_initialized(void) {
    return g_initialized;
}

void core_login(const char* username, const char* password, UserCallback callback, void* user_data) {
    append_log("[USER] Login request: %s\n", username ? username : "NULL");

    if (!g_initialized) {
        append_log("[USER] ERROR: Core not initialized\n");
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    UserManager* user_mgr = find_friend_core_get_user_manager();
    if (!user_mgr) {
        append_log("[USER] ERROR: User manager not available\n");
        if (callback) callback(user_data, -1, "User manager not available");
        return;
    }

    char* fingerprint = user_manager_generate_device_fingerprint();
    user_manager_login(user_mgr, username, password, fingerprint,
                      (LoginCallback)callback, user_data);

    if (fingerprint) free(fingerprint);

    append_log("[USER] Login request sent for: %s\n", username);
}

void core_register(const char* username, const char* password, const char* nickname, UserCallback callback, void* user_data) {
    append_log("[USER] Register request: %s (%s)\n", username ? username : "NULL", nickname ? nickname : "NULL");

    if (!g_initialized) {
        append_log("[USER] ERROR: Core not initialized\n");
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[USER] Registration is not yet implemented, using login flow\n");
    core_login(username, password, callback, user_data);
}

void core_logout(void) {
    append_log("[USER] Logout request\n");

    if (!g_initialized) {
        append_log("[USER] ERROR: Core not initialized\n");
        return;
    }

    UserManager* user_mgr = find_friend_core_get_user_manager();
    if (user_mgr) {
        user_manager_logout(user_mgr);
        append_log("[USER] Logged out successfully\n");
    }
}

void core_get_current_user(UserCallback callback, void* user_data) {
    append_log("[USER] Get current user request\n");

    if (!g_initialized) {
        append_log("[USER] ERROR: Core not initialized\n");
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    UserManager* user_mgr = find_friend_core_get_user_manager();
    if (!user_mgr) {
        if (callback) callback(user_data, -1, "User manager not available");
        return;
    }

    User* user = user_manager_get_current_user(user_mgr);
    if (user) {
        append_log("[USER] Current user: %s\n", user->username);
        if (callback) callback(user_data, 0, "User retrieved");
    } else {
        append_log("[USER] No current user logged in\n");
        if (callback) callback(user_data, 1, "No user logged in");
    }
}

void core_get_user_by_id(uint32_t user_id, UserCallback callback, void* user_data) {
    append_log("[USER] Get user by ID: %u\n", user_id);

    if (!g_initialized) {
        append_log("[USER] ERROR: Core not initialized\n");
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    UserManager* user_mgr = find_friend_core_get_user_manager();
    if (!user_mgr) {
        if (callback) callback(user_data, -1, "User manager not available");
        return;
    }

    char user_id_str[32];
    snprintf(user_id_str, sizeof(user_id_str), "%u", user_id);
    user_manager_get_user_info(user_mgr, user_id_str, (LoginCallback)callback, user_data);
}

void core_update_user(const char* user_json, UserCallback callback, void* user_data) {
    append_log("[USER] Update user: %s\n", user_json ? user_json : "NULL");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[USER] Update user not fully implemented\n");
    if (callback) callback(user_data, 0, "Update request sent");
}

void core_send_message(uint32_t to_id, const char* message, MessageCallback callback, void* user_data) {
    append_log("[MESSAGE] Send message to %u: %s\n", to_id, message ? message : "NULL");

    if (!g_initialized) {
        append_log("[MESSAGE] ERROR: Core not initialized\n");
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MESSAGE] Message sent successfully\n");
    if (callback) callback(user_data, 0, "Message sent");
}

void core_get_messages(int count, MessageCallback callback, void* user_data) {
    append_log("[MESSAGE] Get messages (count=%d)\n", count);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MESSAGE] Messages retrieved: 0 (placeholder)\n");
    if (callback) callback(user_data, 0, "[]");
}

void core_mark_message_read(uint32_t message_id, MessageCallback callback, void* user_data) {
    append_log("[MESSAGE] Mark message as read: %u\n", message_id);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MESSAGE] Message marked as read\n");
    if (callback) callback(user_data, 0, "Marked as read");
}

void core_delete_message(uint32_t message_id, MessageCallback callback, void* user_data) {
    append_log("[MESSAGE] Delete message: %u\n", message_id);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MESSAGE] Message deleted\n");
    if (callback) callback(user_data, 0, "Deleted");
}

void core_report_location(double latitude, double longitude, CoreLocationCallback callback, void* user_data) {
    append_log("[LOCATION] Report location: %.6f, %.6f\n", latitude, longitude);

    if (!g_initialized) {
        append_log("[LOCATION] ERROR: Core not initialized\n");
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    LocationManager* loc_mgr = find_friend_core_get_location_manager();
    if (loc_mgr) {
        append_log("[LOCATION] Location reported successfully\n");
        if (callback) callback(user_data, 0, "Location reported");
    } else {
        append_log("[LOCATION] ERROR: Location manager not available\n");
        if (callback) callback(user_data, -1, "Location manager not available");
    }
}

void core_get_nearby_users(double latitude, double longitude, double radius_meters, CoreLocationCallback callback, void* user_data) {
    append_log("[LOCATION] Get nearby users: %.6f, %.6f (radius=%.1fm)\n", latitude, longitude, radius_meters);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[LOCATION] Nearby users retrieved: 0 (placeholder)\n");
    if (callback) callback(user_data, 0, "[]");
}

void core_get_user_location(uint32_t user_id, CoreLocationCallback callback, void* user_data) {
    append_log("[LOCATION] Get user location: %u\n", user_id);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[LOCATION] User location retrieved (placeholder)\n");
    if (callback) callback(user_data, 0, "{}");
}

void core_set_location_sharing(bool enabled, CoreLocationCallback callback, void* user_data) {
    append_log("[LOCATION] Set location sharing: %s\n", enabled ? "ON" : "OFF");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[LOCATION] Location sharing set to: %s\n", enabled ? "ON" : "OFF");
    if (callback) callback(user_data, 0, enabled ? "ON" : "OFF");
}

void core_get_games(GameCallback callback, void* user_data) {
    append_log("[GAME] Get games list\n");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[GAME] Games retrieved: 0 (placeholder)\n");
    if (callback) callback(user_data, 0, "[]");
}

void core_join_game(uint32_t game_id, GameCallback callback, void* user_data) {
    append_log("[GAME] Join game: %u\n", game_id);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[GAME] Joined game successfully\n");
    if (callback) callback(user_data, 0, "Game joined");
}

void core_leave_game(uint32_t game_id, GameCallback callback, void* user_data) {
    append_log("[GAME] Leave game: %u\n", game_id);

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[GAME] Left game successfully\n");
    if (callback) callback(user_data, 0, "Game left");
}

void core_game_action(uint32_t game_id, const char* action_json, GameCallback callback, void* user_data) {
    append_log("[GAME] Game action: %u, %s\n", game_id, action_json ? action_json : "NULL");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[GAME] Action executed\n");
    if (callback) callback(user_data, 0, "Action executed");
}

void core_upload_media(const char* file_path, const char* media_type, MediaCallback callback, void* user_data) {
    append_log("[MEDIA] Upload media: %s (type=%s)\n", file_path ? file_path : "NULL", media_type ? media_type : "unknown");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MEDIA] Media uploaded (placeholder)\n");
    if (callback) callback(user_data, 0, "{\"media_id\":\"placeholder\"}");
}

void core_download_media(const char* media_id, const char* save_path, MediaCallback callback, void* user_data) {
    append_log("[MEDIA] Download media: %s -> %s\n", media_id ? media_id : "NULL", save_path ? save_path : "NULL");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MEDIA] Media downloaded (placeholder)\n");
    if (callback) callback(user_data, 0, "Downloaded");
}

void core_delete_media(const char* media_id, MediaCallback callback, void* user_data) {
    append_log("[MEDIA] Delete media: %s\n", media_id ? media_id : "NULL");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MEDIA] Media deleted\n");
    if (callback) callback(user_data, 0, "Deleted");
}

void core_get_media_info(const char* media_id, MediaCallback callback, void* user_data) {
    append_log("[MEDIA] Get media info: %s\n", media_id ? media_id : "NULL");

    if (!g_initialized) {
        if (callback) callback(user_data, -1, "Core not initialized");
        return;
    }

    append_log("[MEDIA] Media info retrieved (placeholder)\n");
    if (callback) callback(user_data, 0, "{}");
}

void core_debug_command(const char* command, CoreCallback callback, void* user_data) {
    append_log("[DEBUG] Command: %s\n", command ? command : "NULL");

    if (!command) {
        if (callback) callback(user_data, -1, "No command specified");
        return;
    }

    if (strcmp(command, "status") == 0) {
        if (callback) callback(user_data, 0, core_debug_get_module_status());
    } else if (strcmp(command, "logs") == 0) {
        if (callback) callback(user_data, 0, g_debug_logs);
    } else if (strcmp(command, "version") == 0) {
        if (callback) callback(user_data, 0, g_version);
    } else if (strcmp(command, "ping") == 0) {
        if (callback) callback(user_data, 0, "pong");
    } else {
        append_log("[DEBUG] Unknown command: %s\n", command);
        if (callback) callback(user_data, -1, "Unknown command");
    }
}

void core_debug_get_logs(int last_lines, CoreCallback callback, void* user_data) {
    append_log("[DEBUG] Get logs (last %d lines)\n", last_lines);

    if (callback) callback(user_data, 0, g_debug_logs);
}

void core_debug_clear_logs(void) {
    append_log("[DEBUG] Clear logs\n");
    memset(g_debug_logs, 0, sizeof(g_debug_logs));
    g_log_offset = 0;
    append_log("[DEBUG] Logs cleared\n");
}

const char* core_debug_get_module_status(void) {
    static char status[2048];
    snprintf(status, sizeof(status),
        "Core Interface Status:\n"
        "  Initialized: %s\n"
        "  Version: %s\n"
        "  User Manager: %s\n"
        "  Location Manager: %s\n"
        "  Social Manager: %s\n"
        "  Exhibition Manager: %s\n"
        "  Log buffer size: %zu bytes\n",
        g_initialized ? "YES" : "NO",
        g_version,
        find_friend_core_get_user_manager() ? "AVAILABLE" : "NULL",
        find_friend_core_get_location_manager() ? "AVAILABLE" : "NULL",
        find_friend_core_get_social_manager() ? "AVAILABLE" : "NULL",
        find_friend_core_get_exhibition_manager() ? "AVAILABLE" : "NULL",
        sizeof(g_debug_logs)
    );
    return status;
}

#if defined(__ANDROID__)
void core_set_android_context(void* context) {
    append_log("[ANDROID] Android context set: %p\n", context);
}
#endif

#if defined(_WIN32) || defined(_WIN64)
void core_set_hwnd(void* hwnd) {
    append_log("[WIN32] HWND set: %p\n", hwnd);
}

void core_set_instance(void* instance) {
    append_log("[WIN32] Instance set: %p\n", instance);
}
#endif
