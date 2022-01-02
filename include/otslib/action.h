/**
 * @file action.h
 * @author Abe Kohandel (abe@electronshepherds.com)
 * @brief The module provides the functionality to perform object actions.
 *
 * @copyright Copyright (c) 2022
 */
#ifndef __OTSLIB_ACTION_H__
#define __OTSLIB_ACTION_H__

/**
 * @brief Action
 * @defgroup otslib_action Object Transfer Service Action
 * @ingroup otslib
 * @{
 */

#include <gattlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Truncate Write Mode */
#define OTSLIB_WRITE_MODE_TRUNCATE (1UL << 1)

/**
 * @brief Create Object
 *
 * Create an object with allocated size of @p size and type of @p type.
 * The type is a generic bluetooth UUID type that supports 16-bit, 32-bit,
 * and 128-bit UUIDs. However, only 16-bit and 128-bit UUIDs are valid for
 * an OTS object type.
 *
 * @note Further to the above limitation, currently only 16-byte UUID
 * types are supported.
 *
 * @param adapter otslib adapter
 * @param size object size to create
 * @param type object type to create
 * @retval -EINVAL if @p adapter or @p type are NULL
 *                 if the server does not support the objet type
 * @retval -ENOTSUP if type is not a 16-bit UUID
 *                  if the server does not support object creation
 * @retval -ENOSPC if the server did not have sufficient resources
 * @retval 0 when successful
 * @return other negative values for other errors
 */
int otslib_create(void *adapter, size_t size, uuid_t *type);


/**
 * @brief Delete Object
 *
 * Delete the currently selected object.
 *
 * @param adapter otslib adapter
 * @retval -EINVAL if @p adapter is NULL
 * @retval -ENOTSUP if the server does not support delete
 * @retval -ENOENT if no object is currently selected
 * @retval -EPERM if the current object does not permit deletion
 * @retval -EBUSY if the object is currently busy
 * @retval 0 when successful
 * @return other negative values for other errors
 */
int otslib_delete(void *adapter);

/**
 * @brief Read Object
 *
 * Read @p length at @p offset from the currently selected object.
 *
 * @param adapter otslib_adapter
 * @param offset byte offset to read from
 * @param length number of bytes to read
 * @param[out] buffer pointer to the buffer containing the read data.
 *                    it is the responsibility of the caller to free
 *                    this buffer using free().
 * @retval -EINVAL if @p adapter or @p buffer are NULL
 * @retval -ENOMEM if buffer memory could not be allocated
 * @retval -ENOTSUP if the server does not support read
 * @retval -ENOENT if no object is currently selected
 * @retval -EPERM if the current object does not permit read
 * @retval -EBADR if the offset and length result in an invalid read operation
 * @retval -EBUSY if the object is currently busy
 * @retval -EIO if the read was unexpectedly terminated
 * @return number of bytes read on success
 * @return other negative values for other errors
 */
int otslib_read(void *adapter, off_t offset, size_t length, void **buffer);

/**
 * @brief Write Object
 *
 * Write @p length at @p offset to the currently selected object.
 *
 * @param adapter otslib_adapter
 * @param offset byte offset to write to
 * @param length number of bytes to write
 * @param mode write mode bitmask constructed from @p OTSLIB_WRITE_MODE_* defines
 * @param[out] buffer pointer to the buffer containing the write data.
 * @retval -EINVAL if @p adapter or @p buffer are NULL
 * @retval -ENOTSUP if the server does not support write
 * @retval -ENOENT if no object is currently selected
 * @retval -EPERM if the current object does not permit write
 *                if the patching or truncation was attempted but they are not permitted
 * @retval -EBADR if the offset and length result in an invalid write operation
 * @retval -EBUSY if the object is currently busy
 * @return number of bytes read on success
 * @return other negative values for other errors
 */
int otslib_write(void *adapter, off_t offset, size_t length, unsigned char mode, void *buffer);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_ACTION_H__ */
