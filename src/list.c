#include <errno.h>
#include <gattlib.h>
#include <stdlib.h>

#include "internal.h"
#include "logging.h"
#include "otslib/list.h"

enum otslib_olcp_result {
	OTSLIB_OLCP_RESULT_SUCCESS			= 0x01,
	OTSLIB_OLCP_RESULT_OPCODE_NOT_SUPPORTED		= 0x02,
	OTSLIB_OLCP_RESULT_OPCODE_INVALID_PARAMTER	= 0x03,
	OTSLIB_OLCP_RESULT_OPCODE_OPERATION_FAILED	= 0x04,
	OTSLIB_OLCP_RESULT_OPCODE_OUT_OF_BOUNDS		= 0x05,
	OTSLIB_OLCP_RESULT_OPCODE_TOO_MANY_OBJECTS	= 0x06,
	OTSLIB_OLCP_RESULT_OPCODE_NO_OBJECT		= 0x07,
	OTSLIB_OLCP_RESULT_OPCODE_OBJECT_ID_NOT_FOUND	= 0x08,
	OTSLIB_OLCP_RESULT_MAX,
};

enum otslib_olcp_opcode {
	OTSLIB_OLCP_OPCODE_FIRST			= 0x01,
	OTSLIB_OLCP_OPCODE_LAST				= 0x02,
	OTSLIB_OLCP_OPCODE_PREV				= 0x03,
	OTSLIB_OLCP_OPCODE_NEXT				= 0x04,
	OTSLIB_OLCP_OPCODE_GOTO				= 0x05,
	OTSLIB_OLCP_OPCODE_ORDER			= 0x06,
	OTSLIB_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS	= 0x07,
	OTSLIB_OLCP_OPCODE_CLEAR_MARKING		= 0x08,
	OTSLIB_OLCP_OPCODE_RESPONSE			= 0x70,
};

#define OLCP_FIRST_SIZE 1
#define OLCP_FIRST_DECLARE(name) \
	uint8_t name[OLCP_FIRST_SIZE] = \
	{ OTSLIB_OLCP_OPCODE_FIRST }

#define OLCP_LAST_SIZE 1
#define OLCP_LAST_DECLARE(name) \
	uint8_t name[OLCP_LAST_SIZE] = \
	{ OTSLIB_OLCP_OPCODE_LAST }

#define OLCP_PREV_SIZE 1
#define OLCP_PREV_DECLARE(name) \
	uint8_t name[OLCP_PREV_SIZE] = \
	{ OTSLIB_OLCP_OPCODE_PREV }

#define OLCP_NEXT_SIZE 1
#define OLCP_NEXT_DECLARE(name) \
	uint8_t name[OLCP_NEXT_SIZE] = \
	{ OTSLIB_OLCP_OPCODE_NEXT }

static int error_map[] = {
	EINVAL,
	0,
	ENOTSUP,
	EBADR,
	EFAULT,
	EDOM,
	E2BIG,
	ENOENT,
	EBADF,
};
_Static_assert(OTSLIB_OLCP_RESULT_MAX == ARRAY_DIM(error_map));

static int validate_list_control_notification(struct gatt_notification *notification, void *request, size_t size)
{
	uint8_t *req = (uint8_t *)request;
	uint8_t *resp = (uint8_t *)notification->payload;

	if (size < 1)
		return -EINVAL;

	if (notification->size != 3)
		return -EINVAL;

	LOG(LOG_DEBUG, "List Control Notification: %02x %02x %02x\n",
	    resp[0], resp[1], resp[2]);

	if (resp[0] != OTSLIB_OLCP_OPCODE_RESPONSE) {
		LOG(LOG_ERR, "Unexpected response opcode 0x%02x, expected 0x%02x\n",
		    resp[0], OTSLIB_OLCP_OPCODE_RESPONSE);
		return -EINVAL;
	}

	if (resp[1] != req[0]) {
		LOG(LOG_ERR, "Unexpected response with request opcode 0x%02x, expected 0x%02x\n",
		    resp[1], req[0]);
		return -EINVAL;
	}

	if (resp[2] != OTSLIB_OLCP_RESULT_SUCCESS) {
		LOG(LOG_ERR, "Requested failed with status %u\n",
		    resp[2]);
		if (resp[2] < ARRAY_DIM(error_map))
			return error_map[resp[2]];
		return -EINVAL;
	}

	return 0;
}

static int write_list_control(struct otslib_adapter *adapter, void *buffer, size_t size)
{
	uuid_t uuid;
	int rc = 0;

	rc = start_notification(adapter, &adapter->list, OLCP_UUID16);
	if (rc)
		return rc;

	gattlib_string_to_uuid(OLCP_UUID16, strlen(OLCP_UUID16), &uuid);
	rc = gattlib_write_char_by_uuid(adapter->connection, &uuid, buffer, size);
	if (rc) {
		LOG(LOG_ERR, "Could not write OLCP: %d\n", rc);
		return -map_error(rc);
	}

	rc = wait_for_notification(&adapter->list, OLCP_NOTIFICATION_TIMEOUT_S);
	if (rc)
		return rc;

	LOG(LOG_DEBUG, "Wrote %zu bytes to OLCP.\n", size);

	rc = validate_list_control_notification(&adapter->list, buffer, size);

	free(adapter->list.payload);

	return rc;
}

int otslib_first(void *adapter)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;

	OLCP_FIRST_DECLARE(buffer);

	if (adpt == NULL)
		return -EINVAL;

	return write_list_control(adpt, buffer, sizeof(buffer));
}

int otslib_last(void *adapter)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;

	OLCP_LAST_DECLARE(buffer);

	if (adpt == NULL)
		return -EINVAL;

	return write_list_control(adpt, buffer, sizeof(buffer));
}

int otslib_previous(void *adapter)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;

	OLCP_PREV_DECLARE(buffer);

	if (adpt == NULL)
		return -EINVAL;

	return write_list_control(adpt, buffer, sizeof(buffer));
}

int otslib_next(void *adapter)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;

	OLCP_NEXT_DECLARE(buffer);

	if (adpt == NULL)
		return -EINVAL;

	return write_list_control(adpt, buffer, sizeof(buffer));
}
