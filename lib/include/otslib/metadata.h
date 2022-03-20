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

/** @brief Object Property Delete */
#define OTSLIB_OBJECT_PROPERTY_DELETE (1UL << 0)

/** @brief Object Property Execute */
#define OTSLIB_OBJECT_PROPERTY_EXECUTE (1UL << 1)

/** @brief Object Property Read */
#define OTSLIB_OBJECT_PROPERTY_READ (1UL << 2)

/** @brief Object Property Write */
#define OTSLIB_OBJECT_PROPERTY_WRITE (1UL << 3)

/** @brief Object Property Append */
#define OTSLIB_OBJECT_PROPERTY_APPEND (1UL << 4)

/** @brief Object Property Truncate */
#define OTSLIB_OBJECT_PROPERTY_TRUNCATE (1UL << 5)

/** @brief Object Property Patch */
#define OTSLIB_OBJECT_PROPERTY_PATCH (1UL << 6)

/** @brief Object Property Mark */
#define OTSLIB_OBJECT_PROPERTY_MARK (1UL << 7)

/**
 * @brief Get Object Name
 *
 * The object name returned is dynamically allocated, it is the caller's
 * responsibility to free the memory when no longer needed to prevent
 * a resource leak.
 *
 * @param adapter otslib adapter
 * @param[out] name object's name
 * @return -EINVAL if @p adapter or @p name is NULL
 * @return -ENOMEM if could not allocate memory for @p name
 * @return 0 when successful
 * @return other negative values for other errors
 */
int otslib_get_name(void *adapter, char **name);

/**
 * @brief Set Object Name
 *
 * @param adapter otslib adapter
 * @param name object name to set
 * @return -EINVAL if @p adapter or @p name is NULL, or name length is too long
 * @return 0 when successful
 * @return other negative values for other errors
 */
int otslib_set_name(void *adapter, const char *name);

/**
 * @brief Get Object Type
 *
 * @param adapter otslib adapter
 * @param[out] uuid type uuid
 * @return -EINVAL if @p adapter or @p uuid is NULL
 * @return 0 when successful
 * @return other negative values for other errors
 */
int otslib_type(void *adapter, uuid_t *uuid);

/**
 * @brief Get Object Size
 *
 * @param adapter otslib adapter
 * @param[out] current object's current size
 * @param[out] allocated object's allocated size
 * @return -EINVAL if @p adapter is NULL or both @p current and @p allocated are NULL
 * @return -EIO if object size could not be read
 * @return 0 when successful
 * @return other negative values for other errors
 */
int otslib_size(void *adapter, size_t *current, size_t *allocated);

/**
 * @brief Get Object ID
 *
 * @param adapter otslib adapter
 * @param[out] id object's ID
 * @return -EINVAL if @p adapter or @p id is NULL
 * @return -EIO if object ID could not be read
 * @return 0 when successful
 * @return other negative values for other errors
 */
int otslib_id(void *adapter, unsigned long long *id);

/**
 * @brief Get Object Properties
 *
 * @param adapter otslib adapter
 * @param[out] properties @p OTSLIB_OBJECT_PROPERTY_ bitmask of object's properties
 * @return -EINVAL if @p adapter or @p properties is NULL
 * @return -EIO if object properties could not be read
 * @return 0 when successful
 * @return other negative values for other errors
 */
int otslib_properties(void *adapter, unsigned long *properties);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_METADATA_H__ */
