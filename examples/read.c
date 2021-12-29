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

static void *gattlib_adapter;
static void *otslib_adapter;
static gatt_connection_t *connection;

static void discovered_device(void *adapter, const char *addr, const char *name, void *user_data)
{
}

static void usage(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <device address> <timeout>\n", argv[0]);
		exit(1);
	}

	address = argv[1];
	timeout = atoi(argv[2]);
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

	if (otslib_first(otslib_adapter)) {
		fprintf(stderr, "Failed to select first object.\n");
		exit(1);
	}

	do {
		size_t size;
		if (otslib_size(otslib_adapter, &size, NULL)) {
			fprintf(stderr, "Failed to read size.\n");
			exit(1);
		}
		printf("Object Size: %zu\n", size);

		uint8_t *buffer;
		int rc = otslib_read(otslib_adapter, 0, size, (void **)&buffer);
		if (rc < 0) {
			fprintf(stderr, "Failed to read from object.\n");
			exit(1);
		}
		printf("Object Data:\n");
		hexdump(buffer, rc);
		free(buffer);
	} while (otslib_next(otslib_adapter) == 0);

	teardown();

	return 0;
}
