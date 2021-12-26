#include <otslib/action.h>
#include <otslib/list.h>
#include <otslib/metadata.h>
#include <otslib/otslib.h>
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

	do {
		size_t size;
		if (otslib_size(otslib_adapter, &size, NULL)) {
			fprintf(stderr, "Failed to read size.\n");
			rc = -1;
			goto exit;
		}
		printf("Object Size: %zu\n", size);

		uint8_t *buffer;
		rc = otslib_read(otslib_adapter, 0, size, (void **)&buffer);
		if (rc < 0) {
			fprintf(stderr, "Failed to read from object.\n");
			rc = -1;
			goto exit;
		}
		printf("Object Data:\n");
		hexdump(buffer, rc);
		free(buffer);
	} while (otslib_next(otslib_adapter) == 0);

	rc = 0;

exit:
	if (teardown() < 0)
		exit(1);

	return rc;
}
