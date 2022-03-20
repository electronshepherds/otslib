#include <stdio.h>

#include "logging.h"

void otslib_log(int level, ...)
{
	va_list args;
	FILE *fd = level > LOG_ERR ? stdout : stderr;

	if (level > LOG_LEVEL)
		return;

	va_start(args, level);
	fprintf(fd, LOG_PREFIX);
	vfprintf(fd, va_arg(args, const char *), args);
	va_end(args);
}
