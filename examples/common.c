#include <gattlib.h>
#include <otslib/otslib.h>
#include <stddef.h>

#include "common.h"

static void *gattlib_adapter;
static void *otslib_adapter;
static gatt_connection_t *connection;

static void discovered_device(void *adapter, const char *addr, const char *name, void *user_data)
{
}

void *setup(size_t timeout, const char *address)
{
	if (gattlib_adapter_open(NULL, &gattlib_adapter)) {
		fprintf(stderr, "Failed to open gattlib adapter.\n");
		return NULL;
	}

	if (gattlib_adapter_scan_enable(gattlib_adapter, discovered_device, timeout, NULL)) {
		fprintf(stderr, "Failed to enable scan.\n");
		goto exit;
	}

	if (gattlib_adapter_scan_disable(gattlib_adapter)) {
		fprintf(stderr, "Failed to disable scan.\n");
		goto exit;
	}

	connection = gattlib_connect(gattlib_adapter, address, GATTLIB_CONNECTION_OPTIONS_LEGACY_DEFAULT);
	if (connection == NULL) {
		fprintf(stderr, "Failed to connect to the device.\n");
		goto exit;
	}

	if (otslib_open(connection, address, &otslib_adapter)) {
		fprintf(stderr, "Failed to open otslib adapter.\n");
		otslib_adapter = NULL;
		goto exit;
	}

	return otslib_adapter;

exit:
	if (gattlib_adapter_close(gattlib_adapter))
		fprintf(stderr, "Failed to close gattlib adapter.\n");

	return otslib_adapter;
}

int teardown(void)
{
	int rc = 0;

	if (otslib_close(otslib_adapter)) {
		fprintf(stderr, "Failed to close otslib adapter.\n");
		rc = -1;
	}

	if (gattlib_disconnect(connection)) {
		fprintf(stderr, "Failed to disconnect from device.\n");
		rc = -1;
	}

	return rc;
}
