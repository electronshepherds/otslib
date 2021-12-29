#include <otslib/action.h>
#include <otslib/list.h>
#include <otslib/metadata.h>
#include <otslib/otslib.h>
#include <gattlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "hexdump.h"

static const char *address;
static size_t timeout;
static size_t size;
static uuid_t type;
static const char *name;


static void *gattlib_adapter;
static void *otslib_adapter;
static gatt_connection_t *connection;

static void discovered_device(void *adapter, const char *addr, const char *name, void *user_data)
{
}

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

static void setup(void)
{
	if (gattlib_adapter_open(NULL, &gattlib_adapter)) {
		fprintf(stderr, "Failed to open gattlib adapter.\n");
		exit(1);
	}

	if (gattlib_adapter_scan_enable(gattlib_adapter, discovered_device, timeout, NULL)) {
		fprintf(stderr, "Failed to enable scan.\n");
		exit(1);
	}

	if (gattlib_adapter_scan_disable(gattlib_adapter)) {
		fprintf(stderr, "Failed to disable scan.\n");
		exit(1);
	}

	if (gattlib_adapter_close(gattlib_adapter)) {
		fprintf(stderr, "Failed to close gattlib adapter.\n");
		exit(1);
	}

	connection = gattlib_connect(NULL, address, GATTLIB_CONNECTION_OPTIONS_LEGACY_DEFAULT);
	if (connection == NULL) {
		fprintf(stderr, "Failed to connect to the device.\n");
		exit(1);
	}

	if (otslib_open(connection, address, &otslib_adapter)) {
		fprintf(stderr, "Failed to open otslib adapter.\n");
		exit(1);
	}
}

static void teardown(void)
{
	if (otslib_close(otslib_adapter)) {
		fprintf(stderr, "Failed to close otslib adapter.\n");
		exit(1);
	}

	if (gattlib_disconnect(connection)) {
		fprintf(stderr, "Failed to disconnect from device.\n");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	usage(argc, argv);
	setup();

	if (otslib_create(otslib_adapter, size, &type)) {
		fprintf(stderr, "Failed to create object.\n");
		exit(1);
	}

	if (otslib_set_name(otslib_adapter, name)) {
		fprintf(stderr, "Failed to set object name to %s.\n", name);
		exit(1);
	}

	uint8_t *write_buffer = calloc(1, size);
	if (!write_buffer) {
		fprintf(stderr, "Failed to allocate buffer.\n");
		exit(1);
	}

	for (int i = 0; i < size; i++)
		write_buffer[i] = (uint8_t)i;
	printf("Write Buffer:\n");
	hexdump(write_buffer, size);
	int rc = otslib_write(otslib_adapter, 0, size, 0, write_buffer);
	if (rc < 0) {
		fprintf(stderr, "Failed to write object.\n");
		exit(1);
	}

	void *read_buffer;
	rc = otslib_read(otslib_adapter, 0, size, &read_buffer);
	if (rc < 0) {
		fprintf(stderr, "Failed to read object.\n");
		exit(1);
	}
	printf("Read Buffer:\n");
	hexdump(read_buffer, rc);

	if (rc != size) {
		fprintf(stderr, "Wrote %zu but read %d\n", size, rc);
		exit(1);
	}

	if (0 != memcmp(read_buffer, write_buffer, size)) {
		fprintf(stderr, "Data missmatch between written and read data!\n");
		exit(1);
	}

	free(write_buffer);
	free(read_buffer);

	teardown();

	return 0;
}
