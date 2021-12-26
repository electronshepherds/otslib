#include <otslib/otslib.h>
#include <gattlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static const char *address;
static size_t timeout;

static void *gattlib_adapter;
static struct otslib_adapter *otslib_adapter;
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

	unsigned long features;

	if (otslib_action_features(otslib_adapter, &features)) {
		fprintf(stderr, "Failed to read action features.\n");
		exit(1);
	}
	printf("Action Features: 0x%08lx\n", features);

	if (otslib_list_features(otslib_adapter, &features)) {
		fprintf(stderr, "Failed to read list features.\n");
		exit(1);
	}
	printf("List Features: 0x%08lx\n", features);

	teardown();

	return 0;
}
