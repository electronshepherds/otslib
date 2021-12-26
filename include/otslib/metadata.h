#ifndef __OTSLIB_METADATA_H__
#define __OTSLIB_METADATA_H__

#include <gattlib.h>

struct otslib_adapter;

int otslib_get_name(struct otslib_adapter *adapter, char **name);
int otslib_set_name(struct otslib_adapter *adapter, const char *name);
int otslib_type(struct otslib_adapter *adapter, uuid_t *uuid);
int otslib_size(struct otslib_adapter *adapter, size_t *current, size_t *allocated);
int otslib_id(struct otslib_adapter *adapter, unsigned long long *id);
int otslib_properties(struct otslib_adapter *adapter, unsigned long *properties);

#endif /* __OTSLIB_METADATA_H__ */
