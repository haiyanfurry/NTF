// FindFriend 核心初始化模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool g_core_initialized = false;

static const char* g_error_messages[] = {
    "Success",
    "Invalid parameter",
    "Not initialized",
    "Not found",
    "No memory",
    "Permission denied",
    "Already initialized",
    "Not connected",
    "Timeout",
    "Unknown error",
    "System error",
    "Not implemented"
};

FFResult ff_core_init(void) {
    if (g_core_initialized) {
        return FF_ERROR_ALREADY_INITIALIZED;
    }

    g_core_initialized = true;
    FF_LOG_INFO("Core initialized successfully");
    return FF_OK;
}

FFResult ff_core_destroy(void) {
    if (!g_core_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    g_core_initialized = false;
    FF_LOG_INFO("Core destroyed");
    return FF_OK;
}

bool ff_core_is_initialized(void) {
    return g_core_initialized;
}

FFVersion ff_core_get_version(void) {
    static FFVersion version = {
        .major = 5,
        .minor = 0,
        .patch = 0,
        .build_info = "Release"
    };
    return version;
}

const char* ff_core_get_error_message(FFResult code) {
    switch (code) {
        case FF_OK:
            return "Success";
        case FF_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case FF_ERROR_NO_MEMORY:
            return "No memory";
        case FF_ERROR_NOT_FOUND:
            return "Not found";
        case FF_ERROR_PERMISSION:
            return "Permission denied";
        case FF_ERROR_TIMEOUT:
            return "Timeout";
        case FF_ERROR_NETWORK:
            return "Network error";
        case FF_ERROR_SECURITY:
            return "Security error";
        case FF_ERROR_NOT_INITIALIZED:
            return "Not initialized";
        case FF_ERROR_ALREADY_INITIALIZED:
            return "Already initialized";
        case FF_ERROR_UNSUPPORTED:
            return "Unsupported";
        case FF_ERROR_UNKNOWN:
        default:
            return "Unknown error";
    }
}