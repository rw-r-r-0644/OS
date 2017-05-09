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
#define LOG_LEVEL LOG_DEBUG

#if LOG_ENABLED == 1

void log_init();

/* MACRO for printing module name automatically
 * ...unfortunately that actually includes the whole path and
 * doing each call with a different string makes it quite heavy
 * 
 * Fortunately that's not included in release builds
 */
void __log_printf(char * module_name, log_level_t level, char * fmt, ...);
#define log_printf(l, x, ...) __log_printf(__FILE__ , l, x,  ##__VA_ARGS__)

#if LOG_ERROR < LOG_LEVEL
#define log_error(x, ...)
#else
#define log_error(x, ...) log_printf(LOG_ERROR, x, ##__VA_ARGS__)
#endif

#if LOG_WARNING < LOG_LEVEL
#define log_warning(x, ...)
#else
#define log_warning(x, ...) log_printf(LOG_WARNING, x, ##__VA_ARGS__)
#endif

#if LOG_INFO < LOG_LEVEL
#define log_info(x, ...)
#else
#define log_info(x, ...) log_printf(LOG_INFO, x, ##__VA_ARGS__)
#endif

#if LOG_DEBUG < LOG_LEVEL
#define log_debug(x, ...)
#else
#define log_debug(x, ...) log_printf(LOG_DEBUG, x, ##__VA_ARGS__)
#endif

#else

#define log_init()
#define log_printf(l, x, ...)
#define log_mprintf(m, l, x, ...)
#define log_print(l, x, ...)

#endif