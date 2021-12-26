#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "internal.h"
#include "logging.h"

void notification_handler(const uuid_t *uuid, const uint8_t *data, size_t data_length, void *user_data)
{
	char str[UUID_STR_LEN];
	struct otslib_adapter *adapter = (struct otslib_adapter *)user_data;
	struct gatt_notification *notification;

	gattlib_uuid_to_string(uuid, str, sizeof(str));

	LOG(LOG_DEBUG, "Notification received for UUID %s with length %zu.\n", str, data_length);

	if (gattlib_uuid_cmp(uuid, &adapter->action.uuid) == 0) {
		notification = &adapter->action;
	} else if (gattlib_uuid_cmp(uuid, &adapter->list.uuid) == 0) {
		notification = &adapter->list;
	} else {
		char str[UUID_STR_LEN];
		gattlib_uuid_to_string(uuid, str, UUID_STR_LEN);
		LOG(LOG_ERR, "Notification for unknown UUID %s.\n", str);
		return;
	}

	notification->size = data_length;
	notification->payload = calloc(1, data_length);
	memcpy(notification->payload, data, data_length);

	sem_post(&notification->sem);
}

int start_notification(struct otslib_adapter *adapter, struct gatt_notification *notification, const char *uuid16)
{
	int rc = 0;
	uuid_t uuid;

	if (notification->started) {
		LOG(LOG_DEBUG, "Notification already started for %s.\n", uuid16);
		return 0;
	}

	gattlib_string_to_uuid(uuid16, strlen(uuid16), &uuid);

	/* gattlib notification use UUID16 type */
	if (uuid.type != SDP_UUID16) {
		LOG(LOG_ERR, "Invalid UUID type: %u", uuid.type);
		return -EINVAL;
	}

	if (sem_init(&notification->sem, 0, 0)) {
		LOG(LOG_ERR, "Could not initialize semaphore: %s (%d)", strerror(errno), errno);
		return -errno;
	}

	notification->uuid = uuid;

	rc = gattlib_notification_start(adapter->connection, &uuid);
	if (rc) {
		LOG(LOG_ERR, "Could not start notification for %s: %d\n", uuid16, rc);
		return -map_error(rc);
	}

	notification->started = true;

	LOG(LOG_DEBUG, "Notification started for %s.\n", uuid16);

	return 0;
}

int stop_notification(struct otslib_adapter *adapter, struct gatt_notification *notification)
{
	int rc = 0;
	char str[UUID_STR_LEN];

	gattlib_uuid_to_string(&notification->uuid, str, sizeof(str));

	if (!notification->started) {
		LOG(LOG_DEBUG, "Notification already stopped for %s.\n", str);
		return 0;
	}

	rc = gattlib_notification_stop(adapter->connection, &notification->uuid);
	if (rc) {
		LOG(LOG_ERR, "Could not stop notification for %s: %d\n", str, rc);
		return -map_error(rc);
	}

	LOG(LOG_DEBUG, "Notification stopped for %s.\n", str);

	notification->started = false;

	return 0;
}

int wait_for_notification(struct gatt_notification *notification, time_t seconds)
{
	struct timespec ts;
	int rc = clock_gettime(CLOCK_REALTIME, &ts);

	if (rc) {
		LOG(LOG_ERR, "Could not get time: %s (%d)\n", strerror(errno), errno);
		return -errno;
	}

	ts.tv_sec += seconds;
	rc = sem_timedwait(&notification->sem, &ts);
	if (rc < 0) {
		LOG(LOG_ERR, "Wait on semaphore failed: %s (%d)\n", strerror(errno), errno);
		return -errno;
	}

	return 0;
}

int map_error(int error)
{
	static const int gattlib_error_map[] = {
		0,              /* GATTLIB_SUCCESS */
		EINVAL,         /* GATTLIB_INVALID_PARAMETER */
		ENODEV,         /* GATTLIB_NOT_FOUND */
		ENOMEM,         /* GATTLIB_OUT_OF_MEMORY */
		ENOTSUP,        /* GATTLIB_NOT_SUPPORTED */
		EIO,            /* GATTLIB_DEVICE_ERROR */
		ECOMM,          /* GATTLIB_ERROR_DBUS */
		ECOMM,          /* GATTLIB_ERROR_BLUEZ */
		EFAULT,         /* GATTLIB_ERROR_INTERNAL */
	};

	if (error < ARRAY_DIM(gattlib_error_map))
		return gattlib_error_map[error];
	return EFAULT;
}

int open_l2cap_socket(struct otslib_adapter *adapter)
{
#define L2CAP_CID_OTS 0x0025
	int rc = 0;
	int fd;
	char bas[ADDRESS_STRL_LEN];
	struct sockaddr_l2 bind_addr = {
		.l2_family	= AF_BLUETOOTH,
		.l2_psm		= L2CAP_CID_OTS,
		.l2_bdaddr	= *BDADDR_ANY,
		.l2_cid		= 0,
		.l2_bdaddr_type = BDADDR_LE_RANDOM,
	};
	struct sockaddr_l2 addr = {
		.l2_family	= AF_BLUETOOTH,
		.l2_psm		= L2CAP_CID_OTS,
		.l2_bdaddr	= adapter->socket.bdaddr,
		.l2_cid		= 0,
		.l2_bdaddr_type = BDADDR_LE_RANDOM,
	};

	if (adapter->socket.open)
		return 0;

	fd = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (fd < 0) {
		LOG(LOG_ERR, "Could not create an L2CAP socket: %s (%d)\n",
		    strerror(errno), errno);
		return -errno;
	}
	adapter->socket.fd = fd;
	adapter->socket.open = true;

	rc = bind(fd, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
	if (rc < 0) {
		LOG(LOG_ERR, "Could not bind the L2CAP socket: %s (%d)\n",
		    strerror(errno), errno);
		goto close_socket;
	}

	rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		ba2str(&adapter->socket.bdaddr, bas);
		LOG(LOG_ERR, "Could not connect the L2CAP socket to %s: %s (%d)\n",
		    bas, strerror(errno), errno);
		goto close_socket;
	}

	socklen_t optlen = sizeof(adapter->socket.mtu);
	rc = getsockopt(fd, SOL_BLUETOOTH, BT_SNDMTU, &adapter->socket.mtu, &optlen);
	if (rc < 0) {
		LOG(LOG_ERR, "Could not get L2CAP socket MTU: %s (%d)\n",
		    strerror(errno), errno);
		goto close_socket;
	}

	return 0;

close_socket:
	close_l2cap_socket(adapter);
	return -errno;
}

void close_l2cap_socket(struct otslib_adapter *adapter)
{
	if (!adapter->socket.open)
		return;

	close(adapter->socket.fd);
	adapter->socket.open = false;

	return;
}
