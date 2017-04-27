#pragma once

#include <stdarg.h>

typedef enum
{
	LOG_DEBUG    = 0,
	LOG_INFO     = 1,
	LOG_WARNING  = 2,
	LOG_ERROR    = 3,
	LOG_FATAL    = 4
} log_level_t;

// Enables logging
#define LOG_ENABLED 1

// Minimum log level
#define LOG_LEVEL LOG_INFO

#ifdef LOG_ENABLED

void log_init();

/* MACRO for printing module name automatically
 * ...unfortunately that actually includes the whole path and
 * doing each call with a different string makes it quite heavy
 * 
 * Fortunately that's not included in release builds
 */
void __log_printf(char * module_name, log_level_t level, char * fmt, ...);
#define log_printf(l, x, ...) __log_printf(__FILE__ , l, x,  ##__VA_ARGS__)

#else

#define log_init()
#define log_printf(l, x, ...)
#define log_mprintf(m, l, x, ...)
#define log_print(l, x, ...)

#endif