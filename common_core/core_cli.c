#include "core_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 16

typedef struct {
    const char* command;
    const char* description;
    void (*handler)(int argc, char** argv);
} Command;

static void print_banner(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       FindFriend Core Interface - CLI Debug Console       ║\n");
    printf("║                    Version: %s                      ║\n", core_interface_get_version());
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

static void print_help(int argc, char** argv) {
    printf("Available Commands:\n");
    printf("───────────────────────────────────────────────────────────\n");
    printf("  init <host> <port>    - Initialize core interface\n");
    printf("  destroy               - Destroy core interface\n");
    printf("  status                - Show core status\n");
    printf("\n");
    printf("  login <user> <pass>   - Login user\n");
    printf("  logout                - Logout user\n");
    printf("  current-user          - Get current user\n");
    printf("  user <id>             - Get user by ID\n");
    printf("\n");
    printf("  send <id> <msg>       - Send message\n");
    printf("  messages <count>       - Get messages\n");
    printf("  mark-read <msg_id>     - Mark message as read\n");
    printf("  delete-msg <msg_id>    - Delete message\n");
    printf("\n");
    printf("  report-loc <lat> <lon> - Report location\n");
    printf("  nearby <lat> <lon> <r> - Get nearby users\n");
    printf("  user-loc <user_id>     - Get user location\n");
    printf("  loc-sharing <on|off>   - Toggle location sharing\n");
    printf("\n");
    printf("  games                 - Get game list\n");
    printf("  join-game <id>         - Join game\n");
    printf("  leave-game <id>         - Leave game\n");
    printf("  game-action <id> <json> - Game action\n");
    printf("\n");
    printf("  upload <path> <type>  - Upload media\n");
    printf("  download <id> <path>   - Download media\n");
    printf("  delete-media <id>      - Delete media\n");
    printf("  media-info <id>        - Get media info\n");
    printf("\n");
    printf("  debug <cmd>            - Debug commands (status|logs|version|ping)\n");
    printf("  logs                   - Show debug logs\n");
    printf("  clear-logs             - Clear debug logs\n");
    printf("  help                   - Show this help\n");
    printf("  quit                   - Quit CLI\n");
    printf("───────────────────────────────────────────────────────────\n");
    printf("\n");
}

static void cmd_init(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: init <host> <port>\n");
        return;
    }

    const char* host = argv[1];
    int port = atoi(argv[2]);

    printf("Initializing core interface to %s:%d...\n", host, port);
    if (core_interface_init(host, port)) {
        printf("Core interface initialized successfully!\n");
    } else {
        printf("Failed to initialize core interface!\n");
    }
}

static void cmd_destroy(int argc, char** argv) {
    printf("Destroying core interface...\n");
    core_interface_destroy();
    printf("Core interface destroyed.\n");
}

static void cmd_status(int argc, char** argv) {
    printf("%s", core_debug_get_module_status());
}

static void cmd_login(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: login <username> <password>\n");
        return;
    }

    printf("Logging in as %s...\n", argv[1]);
    core_login(argv[1], argv[2], NULL, NULL);
    printf("Login request sent.\n");
}

static void cmd_logout(int argc, char** argv) {
    printf("Logging out...\n");
    core_logout();
    printf("Logged out.\n");
}

static void cmd_current_user(int argc, char** argv) {
    printf("Getting current user...\n");
    core_get_current_user(NULL, NULL);
}

static void cmd_user(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: user <user_id>\n");
        return;
    }

    uint32_t user_id = (uint32_t)atoi(argv[1]);
    printf("Getting user info for ID %u...\n", user_id);
    core_get_user_by_id(user_id, NULL, NULL);
}

static void cmd_send_message(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: send <user_id> <message>\n");
        return;
    }

    uint32_t to_id = (uint32_t)atoi(argv[1]);
    printf("Sending message to user %u: %s\n", to_id, argv[2]);
    core_send_message(to_id, argv[2], NULL, NULL);
}

static void cmd_messages(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: messages <count>\n");
        return;
    }

    int count = atoi(argv[1]);
    printf("Getting %d messages...\n", count);
    core_get_messages(count, NULL, NULL);
}

static void cmd_mark_read(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: mark-read <message_id>\n");
        return;
    }

    uint32_t msg_id = (uint32_t)atoi(argv[1]);
    printf("Marking message %u as read...\n", msg_id);
    core_mark_message_read(msg_id, NULL, NULL);
}

static void cmd_delete_message(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: delete-msg <message_id>\n");
        return;
    }

    uint32_t msg_id = (uint32_t)atoi(argv[1]);
    printf("Deleting message %u...\n", msg_id);
    core_delete_message(msg_id, NULL, NULL);
}

static void cmd_report_location(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: report-loc <latitude> <longitude>\n");
        return;
    }

    double lat = atof(argv[1]);
    double lon = atof(argv[2]);
    printf("Reporting location: %.6f, %.6f\n", lat, lon);
    core_report_location(lat, lon, NULL, NULL);
}

static void cmd_nearby(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: nearby <latitude> <longitude> <radius>\n");
        return;
    }

    double lat = atof(argv[1]);
    double lon = atof(argv[2]);
    double radius = atof(argv[3]);
    printf("Getting nearby users at %.6f, %.6f (radius=%.1fm)...\n", lat, lon, radius);
    core_get_nearby_users(lat, lon, radius, NULL, NULL);
}

static void cmd_user_location(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: user-loc <user_id>\n");
        return;
    }

    uint32_t user_id = (uint32_t)atoi(argv[1]);
    printf("Getting location for user %u...\n", user_id);
    core_get_user_location(user_id, NULL, NULL);
}

static void cmd_location_sharing(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: loc-sharing <on|off>\n");
        return;
    }

    bool enabled = (strcmp(argv[1], "on") == 0);
    printf("Setting location sharing to %s...\n", enabled ? "ON" : "OFF");
    core_set_location_sharing(enabled, NULL, NULL);
}

static void cmd_games(int argc, char** argv) {
    printf("Getting game list...\n");
    core_get_games(NULL, NULL);
}

static void cmd_join_game(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: join-game <game_id>\n");
        return;
    }

    uint32_t game_id = (uint32_t)atoi(argv[1]);
    printf("Joining game %u...\n", game_id);
    core_join_game(game_id, NULL, NULL);
}

static void cmd_leave_game(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: leave-game <game_id>\n");
        return;
    }

    uint32_t game_id = (uint32_t)atoi(argv[1]);
    printf("Leaving game %u...\n", game_id);
    core_leave_game(game_id, NULL, NULL);
}

static void cmd_game_action(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: game-action <game_id> <action_json>\n");
        return;
    }

    uint32_t game_id = (uint32_t)atoi(argv[1]);
    printf("Executing game action for game %u: %s\n", game_id, argv[2]);
    core_game_action(game_id, argv[2], NULL, NULL);
}

static void cmd_upload_media(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: upload <file_path> <media_type>\n");
        return;
    }

    printf("Uploading media %s (type=%s)...\n", argv[1], argv[2]);
    core_upload_media(argv[1], argv[2], NULL, NULL);
}

static void cmd_download_media(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: download <media_id> <save_path>\n");
        return;
    }

    printf("Downloading media %s to %s...\n", argv[1], argv[2]);
    core_download_media(argv[1], argv[2], NULL, NULL);
}

static void cmd_delete_media(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: delete-media <media_id>\n");
        return;
    }

    printf("Deleting media %s...\n", argv[1]);
    core_delete_media(argv[1], NULL, NULL);
}

static void cmd_media_info(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: media-info <media_id>\n");
        return;
    }

    printf("Getting media info for %s...\n", argv[1]);
    core_get_media_info(argv[1], NULL, NULL);
}

static void cmd_debug(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: debug <command>\n");
        printf("Available debug commands: status, logs, version, ping\n");
        return;
    }

    printf("Executing debug command: %s\n", argv[1]);
    core_debug_command(argv[1], NULL, NULL);
}

static void cmd_logs(int argc, char** argv) {
    printf("=== Debug Logs ===\n");
    core_debug_get_logs(0, NULL, NULL);
    printf("==================\n");
}

static void cmd_clear_logs(int argc, char** argv) {
    printf("Clearing debug logs...\n");
    core_debug_clear_logs();
    printf("Logs cleared.\n");
}

static void debug_callback(void* user_data, int status, const char* message) {
    printf("  [Callback] Status=%d, Message=%s\n", status, message ? message : "NULL");
}

static Command g_commands[] = {
    {"init", "Initialize core interface", cmd_init},
    {"destroy", "Destroy core interface", cmd_destroy},
    {"status", "Show core status", cmd_status},
    {"login", "Login user", cmd_login},
    {"logout", "Logout user", cmd_logout},
    {"current-user", "Get current user", cmd_current_user},
    {"user", "Get user by ID", cmd_user},
    {"send", "Send message", cmd_send_message},
    {"messages", "Get messages", cmd_messages},
    {"mark-read", "Mark message as read", cmd_mark_read},
    {"delete-msg", "Delete message", cmd_delete_message},
    {"report-loc", "Report location", cmd_report_location},
    {"nearby", "Get nearby users", cmd_nearby},
    {"user-loc", "Get user location", cmd_user_location},
    {"loc-sharing", "Toggle location sharing", cmd_location_sharing},
    {"games", "Get game list", cmd_games},
    {"join-game", "Join game", cmd_join_game},
    {"leave-game", "Leave game", cmd_leave_game},
    {"game-action", "Game action", cmd_game_action},
    {"upload", "Upload media", cmd_upload_media},
    {"download", "Download media", cmd_download_media},
    {"delete-media", "Delete media", cmd_delete_media},
    {"media-info", "Get media info", cmd_media_info},
    {"debug", "Debug commands", cmd_debug},
    {"logs", "Show debug logs", cmd_logs},
    {"clear-logs", "Clear debug logs", cmd_clear_logs},
    {"help", "Show help", print_help},
    {"quit", "Quit CLI", NULL},
    {"exit", "Exit CLI", NULL},
};

static void parse_and_execute(const char* input) {
    if (!input || strlen(input) == 0) {
        return;
    }

    char* cmd_copy = strdup(input);
    char* cmd = strtok(cmd_copy, " \t");
    if (!cmd) {
        free(cmd_copy);
        return;
    }

    int argc = 0;
    char* argv[MAX_ARGS];
    argv[argc++] = cmd;

    char* token;
    while ((token = strtok(NULL, " \t")) != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
    }
    argv[argc] = NULL;

    for (size_t i = 0; i < sizeof(g_commands) / sizeof(g_commands[0]); i++) {
        if (strcmp(cmd, g_commands[i].command) == 0) {
            if (g_commands[i].handler == NULL) {
                printf("Exiting CLI...\n");
                free(cmd_copy);
                exit(0);
            }
            g_commands[i].handler(argc, argv);
            free(cmd_copy);
            return;
        }
    }

    printf("Unknown command: %s\n", cmd);
    printf("Type 'help' for available commands.\n");
    free(cmd_copy);
}

int main(int argc, char* argv[]) {
    print_banner();

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            printf("$ %s\n", argv[i]);
            parse_and_execute(argv[i]);
        }
        return 0;
    }

    printf("Interactive mode. Type 'help' for commands, 'quit' to exit.\n\n");

    char input[MAX_INPUT_SIZE];
    while (1) {
        printf("FindFriend> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (strlen(input) > 0) {
            parse_and_execute(input);
        }
    }

    if (core_interface_is_initialized()) {
        printf("Cleaning up...\n");
        core_interface_destroy();
    }

    printf("Goodbye!\n");
    return 0;
}
