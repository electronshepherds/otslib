#include <errno.h>
#include <gattlib.h>
#include <stdlib.h>

#include "otslib/action.h"
#include "internal.h"
#include "logging.h"

enum otslib_oacp_result {
	OTSLIB_OACP_RESULT_SUCCESS			= 0x01,
	OTSLIB_OACP_RESULT_OPCODE_NOT_SUPPORTED		= 0x02,
	OTSLIB_OACP_RESULT_INVALID_PARAMETER		= 0x03,
	OTSLIB_OACP_RESULT_INSUFFICIENT_RESOURCES	= 0x04,
	OTSLIB_OACP_RESULT_INVALID_OBJECT		= 0x05,
	OTSLIB_OACP_RESULT_CHANNEL_UNAVAILABLE		= 0x06,
	OTSLIB_OACP_RESULT_UNSUPPORTED_TYPE		= 0x07,
	OTSLIB_OACP_RESULT_PROCEDURE_NOT_PERMITTED	= 0x08,
	OTSLIB_OACP_RESULT_OBJECT_LOCKED		= 0x09,
	OTSLIB_OACP_RESULT_OPERATION_FAILED		= 0x0a,
	OTSLIB_OACP_RESULT_MAX,
};

enum otslib_oacp_opcode {
	OTSLIB_OACP_OPCODE_CREATE		= 0x01,
	OTSLIB_OACP_OPCODE_DELETE		= 0x02,
	OTSLIB_OACP_OPCODE_CALCULATE_CHECKSUM	= 0x03,
	OTSLIB_OACP_OPCODE_EXECUTE		= 0x04,
	OTSLIB_OACP_OPCODE_READ			= 0x05,
	OTSLIB_OACP_OPCODE_WRITE		= 0x06,
	OTSLIB_OACP_OPCODE_ABORT		= 0x07,
	OTSLIB_OACP_OPCODE_RESPONSE		= 0x60,
};

#define OACP_CREATE16_SIZE 7
#define OACP_CREATE16_DECLARE(name, size, uuid) \
	uint8_t name[OACP_CREATE16_SIZE] = \
	{ OTSLIB_OACP_OPCODE_CREATE }; \
	bt_put_le32(size, &name[1]); \
	bt_put_le16((uuid)->value.uuid16, &name[5]);

#define OACP_DELETE_SIZE 1
#define OACP_DELETE_DECLARE(name) \
	uint8_t name[OACP_DELETE_SIZE] = \
	{ OTSLIB_OACP_OPCODE_DELETE }

#define OACP_READ_SIZE 9
#define OACP_READ_DECLARE(name, offset, length) \
	uint8_t name[OACP_READ_SIZE] = \
	{ OTSLIB_OACP_OPCODE_READ }; \
	bt_put_le32(offset, &name[1]); \
	bt_put_le32(length, &name[5]);

#define OACP_WRITE_SIZE 10
#define OACP_WRITE_DECLARE(name, offset, length, mode) \
	uint8_t name[OACP_WRITE_SIZE] = \
	{ OTSLIB_OACP_OPCODE_WRITE }; \
	bt_put_le32(offset, &name[1]); \
	bt_put_le32(length, &name[5]); \
	name[9] = mode;

static int error_map[] = {
	EINVAL,
	0,
	ENOTSUP,
	EBADR,
	ENOSPC,
	ENOENT,
	ENOSR,
	EINVAL,
	EPERM,
	EBUSY,
	EFAULT,
};
_Static_assert(OTSLIB_OACP_RESULT_MAX == ARRAY_DIM(error_map));

static int validate_action_control_notification(struct gatt_notification *notification, void *request, size_t size)
{
	uint8_t *req = (uint8_t *)request;
	uint8_t *resp = (uint8_t *)notification->payload;

	if (size < 1)
		return -EINVAL;

	if (notification->size != 3)
		return -EINVAL;

	LOG(LOG_DEBUG, "Action Control Notification: %02x %02x %02x\n",
	    resp[0], resp[1], resp[2]);

	if (resp[0] != OTSLIB_OACP_OPCODE_RESPONSE) {
		LOG(LOG_ERR, "Unexpected response opcode 0x%02x, expected 0x%02x\n",
		    resp[0], OTSLIB_OACP_OPCODE_RESPONSE);
		return -EINVAL;
	}

	if (resp[1] != req[0]) {
		LOG(LOG_ERR, "Unexpected response with request opcode 0x%02x, expected 0x%02x\n",
		    resp[1], req[0]);
		return -EINVAL;
	}

	if (resp[2] != OTSLIB_OACP_RESULT_SUCCESS) {
		LOG(LOG_ERR, "Requested failed with status %u\n",
		    resp[2]);
		if (resp[2] < ARRAY_DIM(error_map))
			return -error_map[resp[2]];
		return -EINVAL;
	}

	return 0;
}

static int write_action_control(void *adapter, void *buffer, size_t size)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t uuid;
	int rc = 0;

	rc = start_notification(adpt, &adpt->action, OACP_UUID16);
	if (rc)
		return rc;

	gattlib_string_to_uuid(OACP_UUID16, strlen(OACP_UUID16), &uuid);
	rc = gattlib_write_char_by_uuid(adpt->connection, &uuid, buffer, size);
	if (rc) {
		LOG(LOG_ERR, "Could not write OACP: %d\n", rc);
		return -map_error(rc);
	}

	rc = wait_for_notification(&adpt->action, OACP_NOTIFICATION_TIMEOUT_S);
	if (rc)
		return rc;

	LOG(LOG_DEBUG, "Wrote %zu bytes to OACP.\n", size);

	rc = validate_action_control_notification(&adpt->action, buffer, size);

	free(adpt->action.payload);

	return rc;
}

int otslib_create(void *adapter, size_t size, uuid_t *type)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;

	if (adpt == NULL || type == NULL)
		return -EINVAL;

	if (type->type != SDP_UUID16)
		return -ENOTSUP;

	OACP_CREATE16_DECLARE(buffer, size, type);

	return write_action_control(adpt, buffer, sizeof(buffer));
}

int otslib_delete(void *adapter)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;

	if (adpt == NULL)
		return -EINVAL;

	OACP_DELETE_DECLARE(buffer);

	return write_action_control(adpt, buffer, sizeof(buffer));
}

int otslib_read(void *adapter, off_t offset, size_t length, void **buffer)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	int rc = 0;
	int i = 0;
	ssize_t n = 0;
	uint8_t *b;

	if (adpt == NULL || buffer == NULL)
		return -EINVAL;

	OACP_READ_DECLARE(cmd, offset, length);

	rc = open_l2cap_socket(adpt);
	if (rc)
		return rc;

	rc = write_action_control(adpt, cmd, sizeof(cmd));
	if (rc)
		return rc;

	*buffer = malloc(length);
	if (!*buffer)
		return -ENOMEM;

	b = (uint8_t *)*buffer;
	while (length > 0) {
		LOG(LOG_DEBUG, "Attempt to read %zu bytes from socket\n", length);

		n = read(adpt->socket.fd, b + i, length);
		if (n == 0) {
			LOG(LOG_ERR, "L2CAP socket was closed: %s (%d)\n",
			    strerror(errno), errno);
			close_l2cap_socket(adpt);
			rc = -EIO;
			goto free_resources;
		} else if (n < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				continue;
			LOG(LOG_DEBUG, "Could not read from the L2CAP socket: %s (%d)\n",
			    strerror(errno), errno);

			rc = -errno;
			goto free_resources;
		}

		LOG(LOG_DEBUG, "Read %zd bytes from socket\n", n);

		i += n;
		length -= n;
	}

	return i;

free_resources:
	free(*buffer);
	return rc;
}

int otslib_write(void *adapter, off_t offset, size_t length, unsigned char mode, void *buffer)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	int rc = 0;
	int i = 0;
	ssize_t n = 0;
	uint8_t *b;

	if (adpt == NULL || buffer == NULL)
		return -EINVAL;

	OACP_WRITE_DECLARE(cmd, offset, length, mode);

	rc = open_l2cap_socket(adpt);
	if (rc)
		return rc;

	rc = write_action_control(adpt, cmd, sizeof(cmd));
	if (rc)
		return rc;

	b = (uint8_t *)buffer;
	while (length > 0) {
		LOG(LOG_DEBUG, "%zu bytes left to write to socket\n", length);

		n = write(adpt->socket.fd,
			  b + i,
			  length > adpt->socket.mtu ? adpt->socket.mtu : length);
		if (n < 0) {
			LOG(LOG_ERR, "Could not write to L2CAP socket: %s (%d)\n",
			    strerror(errno), errno);
			close_l2cap_socket(adpt);
			return -errno;
		}

		LOG(LOG_DEBUG, "Wrote %zd bytes to socket\n", n);

		i += n;
		length -= n;
	}

	return i;
}
