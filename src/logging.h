#ifndef __OTSLIB_LOGGING_H
#define __OTSLIB_LOGGING_H

#include <stdarg.h>

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#define LOG_PREFIX "otslib: "

#define LOG(level, ...) \
	otslib_log((level), __VA_ARGS__)

void otslib_log(int level, ...);

#endif
