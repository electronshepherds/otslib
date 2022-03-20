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

/**
 * @brief Read Directory Listing Object
 *
 * Read the Directory Listing Object and return the contents in @p buffer.
 *
 * @param adapter otslib adapter
 * @param[out] buffer pointer to the buffer containing the read data.
 *                    it is the responsibility of the caller to free
 *                    this buffer using free().
 * @retval -EINVAL if @p adapter or @p buffer are NULL
 * @retval other negative values for other errors
 * @retval size of the directory listing object in @p buffer on success
 */
int otslib_directory_read(void *adapter, void **buffer);

/**
 * @brief Iterate Over Directory Listing Entries
 *
 * Iterate over the Directory Listing Entries and call the user provided callback
 * for each entry. Iteration is stopped if the user provided callback returns a value
 * other than 0.
 *
 * @param buffer the entire Directory Listing Object buffer read
 *               by @ref otslib_directory_read
 * @param length the size of the Directory Listing Object in the @p buffer
 *               as returned by @ref otslib_directory_read
 * @param user_data arbitrary user data to be passed to the @p callback
 * @param callback user callback to be called for each entry
 * @retval -EINVAL if @p adapter or @p buffer are NULL
 * @return other negative values for other errors
 * @return 0 on success
 */
int otslib_directory_for_each(void *buffer, size_t length, void *user_data, int (*callback)(const struct otslib_object_metadata *metadata, void *user_data));

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_DIRECTORY_H__ */
