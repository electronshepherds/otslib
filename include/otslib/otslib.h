#ifndef __OTSLIB_H__
#define __OTSLIB_H__

#include <gattlib.h>

struct otslib_adapter;

int otslib_open(gatt_connection_t *connection, const char *dst, struct otslib_adapter **adapter);
int otslib_close(struct otslib_adapter *adapter);

int otslib_action_features(struct otslib_adapter *adapter, unsigned long *features);
int otslib_list_features(struct otslib_adapter *adapter, unsigned long *features);


#endif
