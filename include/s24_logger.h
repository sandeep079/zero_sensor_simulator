#ifndef S24_LOGGER_H
#define S24_LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_FATAL = 4
} log_level_t;

typedef struct {
    bool enabled;
    log_level_t min_level;
    FILE *output_stream;
    bool timestamp_enabled;
} s24_logger_config_t;

/* Initialize logger with config */
void s24_logger_init(s24_logger_config_t *config);

/* Log functions */
void s24_log(log_level_t level, const char *file, int line, 
             const char *format, ...);

/* Convenience macros */
#define LOG_DEBUG(...) s24_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  s24_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  s24_log(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) s24_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) s24_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* Get current timestamp string */
char *s24_get_timestamp(void);

/* Close logger */
void s24_logger_close(void);

#endif // S24_LOGGER_H