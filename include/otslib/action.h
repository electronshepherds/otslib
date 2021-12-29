#ifndef __OTSLIB_ACTION_H__
#define __OTSLIB_ACTION_H__

#include <gattlib.h>
#include <unistd.h>

int otslib_create(void *adapter, size_t size, uuid_t *uuid);
int otslib_delete(void *adapter);
int otslib_read(void *adapter, off_t offset, size_t length, void **buffer);
int otslib_write(void *adapter, off_t offset, size_t length, unsigned char mode, void *buffer);

#endif /* __OTSLIB_ACTION_H__ */
