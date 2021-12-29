#ifndef __OTSLIB_H__
#define __OTSLIB_H__

#include <gattlib.h>

int otslib_open(gatt_connection_t *connection, const char *dst, void **adapter);
int otslib_close(void *adapter);

int otslib_action_features(void *adapter, unsigned long *features);
int otslib_list_features(void *adapter, unsigned long *features);

#endif /* __OTSLIB_H__ */
