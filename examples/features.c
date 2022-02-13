#include <otslib/otslib.h>
#include <gattlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static const char *address;
static size_t timeout;

static void *otslib_adapter;

static void usage(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <device address> <timeout>\n", argv[0]);
		exit(1);
	}
	address = argv[1];
	timeout = atoi(argv[2]);
}

int main(int argc, char *argv[])
{
	int rc = 0;

	usage(argc, argv);

	otslib_adapter = setup(timeout, address);
	if (!otslib_adapter)
		exit(1);

	unsigned long features;

	if (otslib_action_features(otslib_adapter, &features)) {
		fprintf(stderr, "Failed to read action features.\n");
		rc = -1;
		goto exit;
	}
	printf("Action Features: 0x%08lx\n", features);

	if (otslib_list_features(otslib_adapter, &features)) {
		fprintf(stderr, "Failed to read list features.\n");
		rc = -1;
		goto exit;
	}
	printf("List Features: 0x%08lx\n", features);

exit:
	if (teardown() < 0)
		exit(1);

	return rc;
}
