#ifndef __OTSLIB_INTERNAL_H__
#define __OTSLIB_INTERNAL_H__

#include <bluetooth/bluetooth.h>
#include <gattlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

#define UUID_STR_LEN (8 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 12 + 1)
#define ADDRESS_STRL_LEN (2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1)

#define ARRAY_DIM(x) (sizeof(x) / sizeof((x)[0]))

#define OACP_UUID128    "00002ac5-0000-1000-8000-00805f9b34fb"
#define OACP_UUID16     "2ac5"
#define OLCP_UUID128    "00002ac6-0000-1000-8000-00805f9b34fb"
#define OLCP_UUID16     "2ac6"

struct gatt_notification {
	bool		started;
	sem_t		sem;
	uuid_t		uuid;
	uint8_t *	payload;
	size_t		size;
};

struct l2cap_socket {
	bool		open;
	int		fd;
	int		mtu;
	bdaddr_t	bdaddr;
};

struct otslib_adapter {
	gatt_connection_t *		connection;
	struct l2cap_socket		socket;
	struct gatt_notification	action;
	struct gatt_notification	list;
};


void notification_handler(const uuid_t *uuid, const uint8_t *data, size_t data_length, void *user_data);
int start_notification(struct otslib_adapter *adapter, struct gatt_notification *notification, const char *uuid16);
int stop_notification(struct otslib_adapter *adapter, struct gatt_notification *notification);
int wait_for_notification(struct gatt_notification *notification, time_t seconds);
int map_error(int error);
int open_l2cap_socket(struct otslib_adapter *adapter);
void close_l2cap_socket(struct otslib_adapter *adapter);

#endif /* __OTSLIB_INTERNAL_H__ */
