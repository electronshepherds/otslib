#ifndef __OTSLIB_METADATA_H__
#define __OTSLIB_METADATA_H__

#include <gattlib.h>

/**
 * @brief Metadata
 * @defgroup otslib_metadata Object Transfer Service Metadata
 * @ingroup otslib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

int otslib_get_name(void *adapter, char **name);
int otslib_set_name(void *adapter, const char *name);
int otslib_type(void *adapter, uuid_t *uuid);
int otslib_size(void *adapter, size_t *current, size_t *allocated);
int otslib_id(void *adapter, unsigned long long *id);
int otslib_properties(void *adapter, unsigned long *properties);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_METADATA_H__ */