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

	uint8_t *write_buffer = calloc(1, size);
	if (!write_buffer) {
		fprintf(stderr, "Failed to allocate buffer.\n");
		rc = -1;
		goto exit;
	}

	for (int i = 0; i < size; i++)
		write_buffer[i] = (uint8_t)i;
	printf("Write Buffer:\n");
	hexdump(write_buffer, size);
	rc = otslib_write(otslib_adapter, 0, size, 0, write_buffer);
	if (rc < 0) {
		fprintf(stderr, "Failed to write object.\n");
		rc = -1;
		goto exit_write;
	}

	void *read_buffer;
	rc = otslib_read(otslib_adapter, 0, size, &read_buffer);
	if (rc < 0) {
		fprintf(stderr, "Failed to read object.\n");
		rc = -1;
		goto exit_write;
	}
	printf("Read Buffer:\n");
	hexdump(read_buffer, rc);

	if (rc != size) {
		fprintf(stderr, "Wrote %zu but read %d\n", size, rc);
		rc = -1;
		goto exit_read;
	}

	if (0 != memcmp(read_buffer, write_buffer, size)) {
		fprintf(stderr, "Data missmatch between written and read data!\n");
		rc = -1;
		goto exit_read;
	}

	rc = 0;

exit_read:
	free(read_buffer);
exit_write:
	free(write_buffer);
exit:
	if (teardown() < 0)
		exit(1);

	return rc;
}
