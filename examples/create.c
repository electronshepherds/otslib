#include <otslib/action.h>
#include <otslib/metadata.h>
#include <otslib/otslib.h>
#include <gattlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static const char *address;
static size_t timeout;
static size_t size;
static uuid_t type;
static const char *name;

static void *otslib_adapter;

static void usage(int argc, char *argv[])
{
	if (argc != 6) {
		printf("Usage: %s <device address> <timeout> <size> <type> <name>\n", argv[0]);
		exit(1);
	}

	address = argv[1];
	timeout = atoi(argv[2]);
	size = atoi(argv[3]);
	type.type = SDP_UUID16;
	type.value.uuid16 = atoi(argv[4]);
	name = argv[5];
}

int main(int argc, char *argv[])
{
	int rc = 0;

	usage(argc, argv);

	otslib_adapter = setup(timeout, address);
	if (!otslib_adapter)
		exit(1);

	if (otslib_create(otslib_adapter, size, &type)) {
		fprintf(stderr, "Failed to create object.\n");
		rc = -1;
		goto exit;
	}

	if (otslib_set_name(otslib_adapter, name)) {
		fprintf(stderr, "Failed to set object name to %s.\n", name);
		rc = -1;
		goto exit;
	}

	char *read_name;
	if (otslib_get_name(otslib_adapter, &read_name)) {
		fprintf(stderr, "Failed to get object name.\n");
		rc = -1;
		goto exit;
	}
	printf("Name: %s\n", read_name);
	free(read_name);

	size_t current_size, allocated_size;
	if (otslib_size(otslib_adapter, &current_size, &allocated_size)) {
		fprintf(stderr, "Failed to read object size.\n");
		rc = -1;
		goto exit;
	}
	printf("Current Size: %zu\n", current_size);
	printf("Allocated Size: %zu\n", allocated_size);

exit:
	if (teardown() < 0)
		exit(1);

	return rc;
}
