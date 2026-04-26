// FindFriend 日志管理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define MAX_LOG_MESSAGE_LENGTH 1024
#define LOG_FILE "findfriend.log"

static FILE* g_log_file = NULL;
static FFLogLevel g_log_level = FF_LOG_INFO;
static bool g_log_to_console = true;
static bool g_log_to_file = true;
static bool g_log_initialized = false;

static const char* log_level_names[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
};

static void ff_log_write(FFLogLevel level, const char* format, va_list args) {
    if (level < g_log_level) {
        return;
    }

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    char log_line[MAX_LOG_MESSAGE_LENGTH];
    vsnprintf(log_line, sizeof(log_line), format, args);

    if (g_log_to_console) {
        printf("[%s] [%s] %s\n", timestamp, log_level_names[level], log_line);
        fflush(stdout);
    }

    if (g_log_to_file && g_log_file) {
        fprintf(g_log_file, "[%s] [%s] %s\n", timestamp, log_level_names[level], log_line);
        fflush(g_log_file);
    }
}

void ff_log(FFLogLevel level, const char* module, const char* format, ...) {
    va_list args;
    char header[256];
    snprintf(header, sizeof(header), "[%s] %s: ", log_level_names[level], module);

    va_start(args, format);
    vfprintf(stdout, header, args);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    fflush(stdout);

    if (g_log_to_file && g_log_file) {
        vfprintf(g_log_file, header, args);
        vfprintf(g_log_file, format, args);
        fprintf(g_log_file, "\n");
        fflush(g_log_file);
    }
    va_end(args);
}

void ff_log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    ff_log_write(FF_LOG_DEBUG, format, args);
    va_end(args);
}

void ff_log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    ff_log_write(FF_LOG_INFO, format, args);
    va_end(args);
}

void ff_log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    ff_log_write(FF_LOG_WARN, format, args);
    va_end(args);
}

void ff_log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    ff_log_write(FF_LOG_ERROR, format, args);
    va_end(args);
}

void ff_log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    ff_log_write(FF_LOG_ERROR, format, args);
    va_end(args);
}

FFResult ff_log_manager_init(const char* log_file, FFLogLevel level, bool console, bool file) {
    if (log_file) {
        g_log_file = fopen(log_file, "a");
        if (!g_log_file) {
            return FF_ERROR_UNKNOWN;
        }
    } else {
        g_log_file = fopen(LOG_FILE, "a");
        if (!g_log_file) {
            return FF_ERROR_UNKNOWN;
        }
    }

    g_log_level = level;
    g_log_to_console = console;
    g_log_to_file = file;
    g_log_initialized = true;

    return FF_OK;
}

void ff_log_manager_cleanup(void) {
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    g_log_initialized = false;
}

void ff_log_set_level(FFLogLevel level) {
    g_log_level = level;
}

FFResult ff_log_manager_set_level(FFLogLevel level) {
    if (level < FF_LOG_DEBUG || level > FF_LOG_ERROR) {
        return FF_ERROR_INVALID_PARAM;
    }

    g_log_level = level;
    return FF_OK;
}

FFResult ff_log_manager_set_console(bool enable) {
    g_log_to_console = enable;
    return FF_OK;
}

FFResult ff_log_manager_set_file(bool enable) {
    g_log_to_file = enable;
    return FF_OK;
}

FFResult ff_log_manager_rotate(void) {
    if (!g_log_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }

    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = fopen(LOG_FILE, "a");
        if (!g_log_file) {
            return FF_ERROR_UNKNOWN;
        }
    }

    return FF_OK;
}

FFResult ff_log_manager_get_stats(FFLogLevel* level, bool* console, bool* file) {
    if (!level || !console || !file) {
        return FF_ERROR_INVALID_PARAM;
    }

    *level = g_log_level;
    *console = g_log_to_console;
    *file = g_log_to_file;

    return FF_OK;
}