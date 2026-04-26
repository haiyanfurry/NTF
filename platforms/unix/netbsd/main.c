#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "platform_specific.h"

extern int boot_check_init(void);
extern void boot_check_cleanup(void);
extern int security_module_init(void);
extern void security_module_destroy(void);
extern int core_interface_init(const char* server_host, int port);
extern void core_interface_destroy(void);
extern const char* core_interface_get_version(void);
extern int find_friend_core_init(const char* server, int port);
extern void find_friend_core_destroy(void);

static void print_banner(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        FindFriend - BSD/Unix Platform Application           ║\n");
    printf("║                    %s                           ║\n", platform_get_platform_name());
    printf("║              Version: %s                           ║\n", core_interface_get_version());
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

static void platform_specific_init(void) {
    printf("[PLATFORM] Initializing %s platform...\n", platform_get_platform_name());
    platform_init_network();
    platform_log_system_info();
}

static void platform_specific_cleanup(void) {
    printf("[PLATFORM] Cleaning up %s platform...\n", platform_get_platform_name());
    platform_cleanup_network();
}

int main(int argc, char* argv[]) {
    print_banner();

    printf("FindFriend BSD/Unix version starting...\n");

    platform_specific_init();

    printf("\nInitializing all security modules...\n");
    if (security_module_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize security module\n");
        return 1;
    }

    if (!boot_check_init()) {
        fprintf(stderr, "ERROR: Failed to initialize boot check\n");
        security_module_destroy();
        return 1;
    }

    printf("\nInitializing core interface...\n");
    if (!core_interface_init("localhost", 8080)) {
        fprintf(stderr, "ERROR: Failed to initialize core interface\n");
        boot_check_cleanup();
        security_module_destroy();
        return 1;
    }

    printf("\n");
    printf("All security modules initialized successfully ✅\n");
    printf("App initialized successfully ✅\n");
    printf("App running... ✅\n");

    sleep(2);

    printf("\nCleaning up...\n");
    core_interface_destroy();
    boot_check_cleanup();
    security_module_destroy();
    platform_specific_cleanup();

    printf("\n");
    printf("FindFriend %s version exited ✅\n", platform_get_platform_name());
    printf("\n");

    return 0;
}
