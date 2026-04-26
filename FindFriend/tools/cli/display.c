// FindFriend CLI 工具显示模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void ff_cli_print_header(void) {
    printf("==========================================\n");
    printf("        FindFriend CLI Tool v5.0\n");
    printf("==========================================\n\n");
}

void ff_cli_print_footer(void) {
    printf("\n==========================================\n");
}

void ff_cli_print_success(const char* message) {
    printf("[SUCCESS] %s\n", message);
}

void ff_cli_print_error(const char* message) {
    fprintf(stderr, "[ERROR] %s\n", message);
}

void ff_cli_print_warning(const char* message) {
    printf("[WARNING] %s\n", message);
}

void ff_cli_print_info(const char* message) {
    printf("[INFO] %s\n", message);
}

void ff_cli_print_line(void) {
    printf("------------------------------------------\n");
}

void ff_cli_print_table_header(const char* title) {
    printf("%s\n", title);
    ff_cli_print_line();
}

void ff_cli_print_table_row(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void ff_cli_print_progress(const char* message, int progress) {
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;
    
    printf("%s: [", message);
    for (int i = 0; i < 50; i++) {
        if (i < progress / 2) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %d%%\r", progress);
    fflush(stdout);
}

void ff_cli_print_completed(void) {
    printf("\n");
}

FFResult ff_cli_confirm(const char* message) {
    char response[16];
    printf("%s (y/n): ", message);
    if (fgets(response, sizeof(response), stdin) == NULL) {
        return FF_ERROR_UNKNOWN;
    }
    
    if (response[0] == 'y' || response[0] == 'Y') {
        return FF_OK;
    }
    
    return FF_ERROR_UNKNOWN;
}
