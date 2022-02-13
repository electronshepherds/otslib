#include <otslib/list.h>
#include <otslib/metadata.h>
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

	if (otslib_first(otslib_adapter)) {
		fprintf(stderr, "Failed to select first object.\n");
		rc = -1;
		goto exit;
	}

	unsigned long long id;
	if (otslib_id(otslib_adapter, &id)) {
		fprintf(stderr, "Failed to read id.\n");
		rc = -1;
		goto exit;
	}
	printf("ID: 0x%012llx\n", id);

	char *name;
	if (otslib_get_name(otslib_adapter, &name)) {
		fprintf(stderr, "Failed to read name.\n");
		rc = -1;
		goto exit;
	}
	printf("Name: %s\n", name);
	free(name);

	uuid_t uuid;
	if (otslib_type(otslib_adapter, &uuid)) {
		fprintf(stderr, "Failed to read type.\n");
		rc = -1;
		goto exit;
	}
	printf("Type: 0x%04x\n", uuid.value.uuid16);

	size_t current_size;
	size_t allocated_size;
	if (otslib_size(otslib_adapter, &current_size, &allocated_size)) {
		fprintf(stderr, "Failed to read size.\n");
		rc = -1;
		goto exit;
	}
	printf("Current Size: %zu\n", current_size);
	printf("Allocated Size: %zu\n", allocated_size);

	unsigned long properties;
	if (otslib_properties(otslib_adapter, &properties)) {
		fprintf(stderr, "Failed to read properties.\n");
		rc = -1;
		goto exit;
	}
	printf("Properties: 0x%08lx\n", properties);

exit:
	if (teardown() < 0)
		exit(1);

	return rc;
}
