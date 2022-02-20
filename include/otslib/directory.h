#ifndef __OTSLIB_DIRECTORY_H__
#define __OTSLIB_DIRECTORY_H__

/**
 * @brief Directory
 * @defgroup otslib_directory Object Transfer Service Directory
 * @ingroup otslib
 * @{
 */

#include <stdint.h>

#include "otslib/otslib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OTSLIB_DIRECTORY_FLAG_OBJ_TYPE_UUID128 (1UL << 0)
#define OTSLIB_DIRECTORY_FLAG_CURRENT_SIZE (1UL << 1)
#define OTSLIB_DIRECTORY_FLAG_ALLOCATED_SIZE (1UL << 2)
#define OTSLIB_DIRECTORY_FLAG_FIRST_CREATED (1UL << 3)
#define OTSLIB_DIRECTORY_FLAG_LAST_MODIFIED (1UL << 4)
#define OTSLIB_DIRECTORY_FLAG_PROPERTIES (1UL << 5)
#define OTSLIB_DIRECTORY_FLAG_EXTENDED (1UL << 7)

int otslib_directory_read(void *adapter, void **buffer);
int otslib_directory_for_each(void *buffer, size_t length, void *user_data, int (*callback)(const struct otslib_object_metadata *metadata, void *user_data));

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_DIRECTORY_H__ */
