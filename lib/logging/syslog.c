#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "logging.h"

static const int level_to_syslog[] = {
	LOG_EMERG,
	LOG_ALERT,
	LOG_CRIT,
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG
};

void otslib_log(int level, ...)
{
	va_list args;

	if (level > LOG_LEVEL)
		return;

	va_start(args, level);

	const char *fmt = va_arg(args, const char *);

	char *tmp = calloc(1, strlen(LOG_PREFIX) + strlen(fmt) + 1);
	if (tmp != NULL) {
		sprintf(tmp, "%s%s", LOG_PREFIX, fmt);
		fmt = tmp;
	}

	vsyslog(level_to_syslog[level], fmt, args);

	free(tmp);

	va_end(args);
}
