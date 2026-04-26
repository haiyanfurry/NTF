#define _GNU_SOURCE

#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void free_auth_token(AuthToken* token);
static AuthToken* copy_auth_token(const AuthToken* source);

static void free_auth_token(AuthToken* token) {
    if (token) {
        if (token->access_token) free(token->access_token);
        if (token->refresh_token) free(token->refresh_token);
        if (token->token_type) free(token->token_type);
        if (token->scope) free(token->scope);
        free(token);
    }
}

static AuthToken* copy_auth_token(const AuthToken* source) {
    if (!source) {
        return NULL;
    }
    
    AuthToken* dest = (AuthToken*)malloc(sizeof(AuthToken));
    if (!dest) {
        return NULL;
    }
    
    dest->access_token = source->access_token ? strdup(source->access_token) : NULL;
    dest->refresh_token = source->refresh_token ? strdup(source->refresh_token) : NULL;
    dest->token_type = source->token_type ? strdup(source->token_type) : NULL;
    dest->expires_at = source->expires_at;
    dest->scope = source->scope ? strdup(source->scope) : NULL;
    
    return dest;
}

AuthManager* auth_manager_init() {
    AuthManager* manager = (AuthManager*)malloc(sizeof(AuthManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory for auth manager\n");
        return NULL;
    }
    
    manager->current_token = NULL;
    manager->status = AUTH_STATUS_NOT_AUTHENTICATED;
    manager->last_error = AUTH_ERROR_NONE;
    manager->network_manager = NULL;
    
    return manager;
}

void auth_manager_destroy(AuthManager* manager) {
    if (manager) {
        auth_manager_logout(manager);
        free(manager);
    }
}

bool auth_manager_authenticate(AuthManager* manager, const char* username, const char* password, AuthCallback callback, void* user_data) {
    if (!manager || !username || !password) {
        return false;
    }
    
    manager->status = AUTH_STATUS_AUTHENTICATING;
    manager->last_error = AUTH_ERROR_NONE;
    
    printf("Authenticating user: %s\n", username);
    
    time_t now = time(NULL);
    
    AuthToken temp_token;
    temp_token.access_token = "access_token_123456";
    temp_token.refresh_token = "refresh_token_123456";
    temp_token.token_type = "Bearer";
    temp_token.expires_at = now + 3600;
    temp_token.scope = "read write";
    
    AuthToken* token = copy_auth_token(&temp_token);
    if (!token) {
        manager->status = AUTH_STATUS_FAILED;
        manager->last_error = AUTH_ERROR_UNKNOWN;
        if (callback) {
            callback(user_data, AUTH_STATUS_FAILED, AUTH_ERROR_UNKNOWN, NULL);
        }
        return false;
    }
    
    if (manager->current_token) {
        free_auth_token(manager->current_token);
    }
    manager->current_token = token;
    manager->status = AUTH_STATUS_AUTHENTICATED;
    
    printf("Authentication successful for user: %s\n", username);
    
    if (callback) {
        callback(user_data, AUTH_STATUS_AUTHENTICATED, AUTH_ERROR_NONE, token);
    }
    
    return true;
}

bool auth_manager_refresh_token(AuthManager* manager, AuthCallback callback, void* user_data) {
    if (!manager || !manager->current_token) {
        return false;
    }
    
    printf("Refreshing authentication token...\n");
    
    time_t now = time(NULL);
    
    AuthToken temp_token;
    temp_token.access_token = "access_token_123457";
    temp_token.refresh_token = "refresh_token_123457";
    temp_token.token_type = "Bearer";
    temp_token.expires_at = now + 3600;
    temp_token.scope = manager->current_token->scope;
    
    AuthToken* token = copy_auth_token(&temp_token);
    if (!token) {
        manager->status = AUTH_STATUS_FAILED;
        manager->last_error = AUTH_ERROR_UNKNOWN;
        if (callback) {
            callback(user_data, AUTH_STATUS_FAILED, AUTH_ERROR_UNKNOWN, NULL);
        }
        return false;
    }
    
    if (manager->current_token) {
        free_auth_token(manager->current_token);
    }
    manager->current_token = token;
    manager->status = AUTH_STATUS_AUTHENTICATED;
    
    printf("Token refreshed successfully\n");
    
    if (callback) {
        callback(user_data, AUTH_STATUS_AUTHENTICATED, AUTH_ERROR_NONE, token);
    }
    
    return true;
}

void auth_manager_logout(AuthManager* manager) {
    if (manager) {
        if (manager->current_token) {
            free_auth_token(manager->current_token);
            manager->current_token = NULL;
        }
        manager->status = AUTH_STATUS_NOT_AUTHENTICATED;
        manager->last_error = AUTH_ERROR_NONE;
        printf("User logged out\n");
    }
}

AuthStatus auth_manager_get_status(AuthManager* manager) {
    if (!manager) {
        return AUTH_STATUS_NOT_AUTHENTICATED;
    }
    return manager->status;
}

AuthToken* auth_manager_get_token(AuthManager* manager) {
    if (!manager) {
        return NULL;
    }
    return manager->current_token;
}

bool auth_manager_is_token_valid(AuthManager* manager) {
    if (!manager || !manager->current_token) {
        return false;
    }
    
    time_t now = time(NULL);
    return manager->current_token->expires_at > now;
}

bool auth_manager_save_token(AuthManager* manager, const char* file_path) {
    if (!manager || !manager->current_token || !file_path) {
        return false;
    }
    
    FILE* file = fopen(file_path, "w");
    if (!file) {
        fprintf(stderr, "Error opening token file for writing: %s\n", file_path);
        return false;
    }
    
    fprintf(file, "access_token=%s\n", manager->current_token->access_token);
    fprintf(file, "refresh_token=%s\n", manager->current_token->refresh_token);
    fprintf(file, "token_type=%s\n", manager->current_token->token_type);
    fprintf(file, "expires_at=%lld\n", (long long)manager->current_token->expires_at);
    fprintf(file, "scope=%s\n", manager->current_token->scope ? manager->current_token->scope : "");
    
    fclose(file);
    printf("Token saved to: %s\n", file_path);
    
    return true;
}

bool auth_manager_load_token(AuthManager* manager, const char* file_path) {
    if (!manager || !file_path) {
        return false;
    }
    
    FILE* file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Error opening token file for reading: %s\n", file_path);
        return false;
    }
    
    AuthToken* token = (AuthToken*)malloc(sizeof(AuthToken));
    if (!token) {
        fclose(file);
        return false;
    }
    
    memset(token, 0, sizeof(AuthToken));
    
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char* eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char* value = eq + 1;
            
            while (*value && (*value == ' ' || *value == '\t')) value++;
            size_t len = strlen(value);
            while (len > 0 && (value[len-1] == '\n' || value[len-1] == '\r')) {
                value[len-1] = '\0';
                len--;
            }
            
            if (strcmp(line, "access_token") == 0) {
                token->access_token = strdup(value);
            } else if (strcmp(line, "refresh_token") == 0) {
                token->refresh_token = strdup(value);
            } else if (strcmp(line, "token_type") == 0) {
                token->token_type = strdup(value);
            } else if (strcmp(line, "expires_at") == 0) {
                token->expires_at = atoll(value);
            } else if (strcmp(line, "scope") == 0) {
                token->scope = (strlen(value) > 0) ? strdup(value) : NULL;
            }
        }
    }
    
    fclose(file);
    
    if (manager->current_token) {
        free_auth_token(manager->current_token);
    }
    manager->current_token = token;
    manager->status = auth_manager_is_token_valid(manager) ? AUTH_STATUS_AUTHENTICATED : AUTH_STATUS_NOT_AUTHENTICATED;
    
    printf("Token loaded from: %s\n", file_path);
    
    return true;
}
