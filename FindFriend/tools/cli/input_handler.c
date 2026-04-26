// FindFriend CLI 工具输入处理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FFResult ff_cli_read_input(char* buffer, size_t buffer_size) {
    if (!buffer) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    printf("> ");
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return FF_ERROR_UNKNOWN;
    }
    
    // 去除换行符
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return FF_OK;
}

FFResult ff_cli_parse_command(char* input, char** command, char*** args, int* argc) {
    if (!input || !command || !args || !argc) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *argc = 0;
    
    // 分割命令和参数
    char* token = strtok(input, " ");
    if (!token) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    *command = strdup(token);
    if (!*command) {
        return FF_ERROR_NO_MEMORY;
    }
    
    // 分配参数数组
    *args = (char**)malloc(16 * sizeof(char*));
    if (!*args) {
        free(*command);
        return FF_ERROR_NO_MEMORY;
    }
    
    // 解析参数
    while ((token = strtok(NULL, " ")) != NULL) {
        if (*argc >= 15) {
            // 扩展参数数组
            char** new_args = (char**)realloc(*args, (*argc + 16) * sizeof(char*));
            if (!new_args) {
                free(*command);
                for (int i = 0; i < *argc; i++) {
                    free((*args)[i]);
                }
                free(*args);
                return FF_ERROR_NO_MEMORY;
            }
            *args = new_args;
        }
        
        (*args)[*argc] = strdup(token);
        if (!(*args)[*argc]) {
            free(*command);
            for (int i = 0; i < *argc; i++) {
                free((*args)[i]);
            }
            free(*args);
            return FF_ERROR_NO_MEMORY;
        }
        
        (*argc)++;
    }
    
    return FF_OK;
}

void ff_cli_free_command(char* command, char** args, int argc) {
    if (command) {
        free(command);
    }
    
    if (args) {
        for (int i = 0; i < argc; i++) {
            if (args[i]) {
                free(args[i]);
            }
        }
        free(args);
    }
}

FFResult ff_cli_get_integer(const char* prompt, int* value) {
    if (!prompt || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    char buffer[64];
    printf("%s: ", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return FF_ERROR_UNKNOWN;
    }
    
    *value = atoi(buffer);
    return FF_OK;
}

FFResult ff_cli_get_string(const char* prompt, char* buffer, size_t buffer_size) {
    if (!prompt || !buffer) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    printf("%s: ", prompt);
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return FF_ERROR_UNKNOWN;
    }
    
    // 去除换行符
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return FF_OK;
}

FFResult ff_cli_get_boolean(const char* prompt, bool* value) {
    if (!prompt || !value) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    char buffer[16];
    printf("%s (y/n): ", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return FF_ERROR_UNKNOWN;
    }
    
    *value = (buffer[0] == 'y' || buffer[0] == 'Y');
    return FF_OK;
}
