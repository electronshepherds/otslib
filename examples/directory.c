#include <otslib/otslib.h>
#include <otslib/directory.h>
#include <gattlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "hexdump.h"

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

static int directory_callback(const struct otslib_object_metadata *metadata, void *user_data)
{
	printf("== Name: %s\n", metadata->name);
	printf("   ID: 0x%012lx\n", metadata->id);
	printf("   Type: 0x%04x\n", metadata->type.value.uuid16);
	if (metadata->current_size != -1)
		printf("   Current Size: %zd\n", metadata->current_size);
	if (metadata->allocated_size != -1)
		printf("   Allocated Size: %zd\n", metadata->allocated_size);
	printf("   Properties: 0x%08x\n", metadata->properties);

	return 0;
}

int main(int argc, char *argv[])
{
	int rc = 0;

	usage(argc, argv);

	otslib_adapter = setup(timeout, address);
	if (!otslib_adapter)
		exit(1);

	void *buffer;
	rc = otslib_directory_read(otslib_adapter, &buffer);
	if (rc < 0) {
		fprintf(stderr, "Failed to read directory object.\n");
		rc = -1;
		goto exit;
	}

	size_t len = rc;
	rc = otslib_directory_for_each(buffer, len, NULL, directory_callback);
	if (rc < 0) {
		fprintf(stderr, "Error during entry read.\n");
		hexdump(buffer, len);
	}
	free(buffer);

	rc = 0;

exit:
	if (teardown() < 0)
		exit(1);

	return rc;
}
