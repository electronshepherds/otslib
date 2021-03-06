#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "internal.h"
#include "logging.h"
#include "otslib/otslib.h"

#define FEATURES_UUID   "00002abd-0000-1000-8000-00805f9b34fb"
#define FEATURES_SIZE   8

static int read_features(gatt_connection_t *connection, unsigned long features[2])
{
	uuid_t uuid;
	void *buffer;
	size_t size;
	int rc = 0;

	gattlib_string_to_uuid(FEATURES_UUID, strlen(FEATURES_UUID), &uuid);

	rc = gattlib_read_char_by_uuid(connection, &uuid, &buffer, &size);
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not read OTS Features Characteristic: %s (%d)\n",
		    strerror(rc), rc);
		rc = -rc;
		goto exit;
	}

	if (size != FEATURES_SIZE) {
		LOG(LOG_ERR, "Unexpected OTS Features Characteristic length %zu, expected %zu\n",
		    size, FEATURES_SIZE);
		rc = -EIO;
		goto free_memory;
	}

	features[0] = bt_get_le32(buffer);
	features[1] = bt_get_le32((void *)((uintptr_t)buffer + 4));

	LOG(LOG_DEBUG, "OACP Features (0x%08lx), OLCP Features (0x%08lx)\n",
	    features[0], features[1]);

free_memory:
	free(buffer);
exit:
	return rc;
}

int otslib_open(gatt_connection_t *connection, const char *address, void **adapter)
{
	struct otslib_adapter **adpt = (struct otslib_adapter **)adapter;

	if (connection == NULL || address == NULL || adpt == NULL)
		return -EINVAL;

	*adpt = calloc(1, sizeof(struct otslib_adapter));
	if (*adpt == NULL) {
		LOG(LOG_ERR, "Could not allocate memory for adapter: %s (%d).\n",
		    strerror(ENOMEM), ENOMEM);
		return -ENOMEM;
	}

	(*adpt)->connection = connection;

	if (str2ba(address, &(*adpt)->socket.bdaddr)) {
		LOG(LOG_ERR, "Invalid address: %s.\n", address);
		return -EINVAL;
	}

	gattlib_register_notification(connection, notification_handler, *adpt);

	return 0;
}

int otslib_close(void *adapter)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	int rc = 0;

	if (adpt == NULL)
		return -EINVAL;

	rc = stop_notification(adpt, &adpt->list);
	if (rc)
		return rc;

	rc = stop_notification(adpt, &adpt->action);
	if (rc)
		return rc;

	close_l2cap_socket(adpt);

	free(adpt);

	return 0;
}

int otslib_action_features(void *adapter, unsigned long *features)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	unsigned long value[2];
	int rc = 0;

	if (adpt == NULL || features == NULL)
		return -EINVAL;

	rc = read_features(adpt->connection, value);
	if (rc)
		return rc;

	*features = value[0];

	return 0;
}

int otslib_list_features(void *adapter, unsigned long *features)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	unsigned long value[2];
	int rc = 0;

	if (adpt == NULL || features == NULL)
		return -EINVAL;

	rc = read_features(adpt->connection, value);
	if (rc)
		return rc;

	*features = value[1];

	return 0;
}
