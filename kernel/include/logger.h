#ifndef LOGGER_H
#define LOGGER_H
#define LOG_LEVEL_NONE 0x0
#define LOG_LEVEL_FATAL 0x1
#define LOG_LEVEL_CRITICAL 0x2
#define LOG_LEVEL_ERROR 0x3
#define LOG_LEVEL_WARNING 0x4
#define LOG_LEVEL_INFORMATION 0x5
#define LOG_LEVEL_DEBUG 0x6
#define LOG_LEVEL_TRACE 0x7
#include <kernel/types.h>
typedef unsigned char logger_level_t;
void logger_install(logger_level_t minimum_level);
void log_fatal(string_t str,...);
void log_critical(string_t str,...);
void log_error(string_t str,...);
void log_warning(string_t str,...);
void log_information(string_t str,...);
void log_debug(string_t str,...);
void log_trace(string_t str,...);
#endif