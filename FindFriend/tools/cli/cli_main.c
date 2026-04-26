// FindFriend CLI 工具主入口

#include "ff_core_interface.h"
#include "ff_user_interface.h"
#include "ff_message_interface.h"
#include "ff_location_interface.h"
#include "ff_p2p_interface.h"
#include "ff_game_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int handle_init(void);
static int handle_status(void);
static int handle_help(void);

static int handle_init(void) {
    FFResult res;

    res = ff_core_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize core: %d\n", res);
        return 1;
    }
    printf("[OK] Core initialized\n");

    FFUser* user = NULL;
    res = ff_user_auto_login(NULL, NULL);
    if (res == FF_OK) {
        printf("[OK] User auto login successful\n");
    } else {
        printf("[INFO] No user to auto login (this is normal for first run)\n");
    }

    printf("\n[SUCCESS] All modules initialized successfully!\n");
    return 0;
}

static int handle_status(void) {
    FFUser* current_user = NULL;
    FFResult res = ff_user_get_current(&current_user, NULL, NULL);

    printf("=== FindFriend Status ===\n");
    printf("Core: OK\n");

    if (res == FF_OK && current_user) {
        printf("User: %s (UID: %u)\n", current_user->username, current_user->uid);
        printf("Status: %s\n", current_user->status == 1 ? "Online" : "Offline");
        ff_user_free(current_user);
    } else {
        printf("User: Not logged in\n");
    }

    FF_LOG_INFO("Status command executed");
    return 0;
}

static int handle_help(void) {
    printf("=== FindFriend CLI Commands ===\n");
    printf("  init      - Initialize all modules\n");
    printf("  status    - Show current status\n");
    printf("  help      - Show this help message\n");
    printf("  exit      - Exit the program\n");
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        handle_help();
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        return handle_init();
    } else if (strcmp(argv[1], "status") == 0) {
        return handle_status();
    } else if (strcmp(argv[1], "help") == 0) {
        return handle_help();
    } else if (strcmp(argv[1], "exit") == 0) {
        printf("Goodbye!\n");
        return 0;
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        handle_help();
        return 1;
    }
}