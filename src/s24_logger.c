#include "../include/s24_logger.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

static s24_logger_config_t g_config;

void s24_logger_init(s24_logger_config_t *config) {
    if (config) {
        memcpy(&g_config, config, sizeof(s24_logger_config_t));
    } else {
        // Default configuration
        g_config.enabled = true;
        g_config.min_level = LOG_LEVEL_DEBUG;
        g_config.output_stream = stdout;
        g_config.timestamp_enabled = true;
    }
}

char *s24_get_timestamp(void) {
    static char buffer[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    if (tm_info) {
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    } else {
        snprintf(buffer, sizeof(buffer), "1970-01-01 00:00:00");
    }
    return buffer;
}

void s24_log(log_level_t level, const char *file, int line, 
             const char *format, ...) {
    if (!g_config.enabled || level < g_config.min_level) return;

    const char *level_names[] = {
        "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };

    // Get timestamp
    char *ts = s24_get_timestamp();

    // Format message
    char msg_buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(msg_buffer, sizeof(msg_buffer), format, args);
    va_end(args);

    // Output
    fprintf(g_config.output_stream, "[%s] [%s] %s:%d - %s\n",
            ts, level_names[level], file, line, msg_buffer);
    fflush(g_config.output_stream);
}

void s24_logger_close(void) {
    if (g_config.output_stream && g_config.output_stream != stdout) {
        fclose(g_config.output_stream);
    }
}