#ifndef __OTSLIB_ACTION_H__
#define __OTSLIB_ACTION_H__

#include <gattlib.h>
#include <unistd.h>

struct otslib_adapter;

int otslib_create(struct otslib_adapter *adapter, size_t size, uuid_t *uuid);
int otslib_delete(struct otslib_adapter *adapter);
int otslib_read(struct otslib_adapter *adapter, off_t offset, size_t length, void **buffer);
int otslib_write(struct otslib_adapter *adapter, off_t offset, size_t length, unsigned char mode, void *buffer);

#endif /* __OTSLIB_ACTION_H__ */
