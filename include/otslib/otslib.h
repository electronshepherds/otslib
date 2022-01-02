/**
 * @file otslib.h
 * @author Abe Kohandel (abe@electronshepherds.com)
 * @brief This module provides the open and close functionality for the library.
 * It also provides the functionality needed to interrogate the server's
 * features.
 *
 * @copyright Copyright (c) 2022
 */

#ifndef __OTSLIB_H__
#define __OTSLIB_H__

/**
 * @brief Object Transfer Service
 * @defgroup otslib Object Transfer Service
 * @{
 */

#include <gattlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Action Feature Create */
#define OTSLIB_ACTION_FEATURE_CREATE (1UL << 0)

/** @brief Action Feature Delete */
#define OTSLIB_ACTION_FEATURE_DELETE (1UL << 1)

/** @brief Action Feature Calculate Checksum */
#define OTSLIB_ACTION_FEATURE_CHECKSUM (1UL << 2)

/** @brief Action Feature Execute */
#define OTSLIB_ACTION_FEATURE_EXECUTE (1UL << 3)

/** @brief Action Feature Read */
#define OTSLIB_ACTION_FEATURE_READ (1UL << 4)

/** @brief Action Feature Write */
#define OTSLIB_ACTION_FEATURE_WRITE (1UL << 5)

/** @brief Action Feature Appending Additional Data */
#define OTSLIB_ACTION_FEATURE_APPEND (1UL << 6)

/** @brief Action Feature Truncate */
#define OTSLIB_ACTION_FEATURE_TRUNCATE (1UL << 7)

/** @brief Action Feature Patch */
#define OTSLIB_ACTION_FEATURE_PATCH (1UL << 8)

/** @brief Action Feature Abort */
#define OTSLIB_ACTION_FEATURE_ABORT (1UL << 9)

/** @brief List Feature Go To */
#define OTSLIB_LIST_FEATURE_GOTO (1UL << 0)

/** @brief List Feature Order */
#define OTSLIB_LIST_FEATURE_ORDER (1UL << 2)

/** @brief List Feature Request Number of Objects */
#define OTSLIB_LIST_FEATURE_NUMBER_OBJECTS (1UL << 3)

/** @brief List Feature Clear Marking */
#define OTSLIB_LIST_FEATURE_CLEAR_MARKING (1UL << 4)

/**
 * @brief Open otslib Adapter
 *
 * Open an otslib adapter that can bue used for further interactions with otslib.
 * It is responsibility of the user to close the adapter using @ref otslib_close
 * to prevent a resource leak.
 *
 * @param connection connection handle provided by <a href="https://github.com/labapart/gattlib">gattlib</a>
 * @param dst bluetooth address of the device to open an adapter for
 * @param[out] adapter opened adapter
 * @retval -EINVAL if any of the input parameters are NULL
 *                 if the provided device address by @p dst is invalid
 * @retval -ENOMEM if could not allocate memory for the adapter
 * @retval 0 when successful
 */
int otslib_open(gatt_connection_t *connection, const char *dst, void **adapter);

/**
 * @brief Close otslib Adapter
 *
 * Close an otslib adapter previously opened by @ref otslib_open. All allocated
 * resources for this adapter are freed.
 *
 * @param adapter an otslib adapter
 * @retval -EINVAL if @p adapter is NULL
 * @retval 0 when successful
 * @return other negative values for other errors
 */
int otslib_close(void *adapter);

/**
 * @brief Read Action Features
 *
 * Read's the action features of the OTS service represented by @p adapter.
 *
 * @param adapter otslib adapter
 * @param[out] features @p OTSLIB_ACTION_FEATURE_ bitmask of action features
 * @retval -EINVAL if the input parameters are NULL
 * @retval 0 when successful
 * @return other negative values for other errors
 */
int otslib_action_features(void *adapter, unsigned long *features);

/**
 * @brief Read List Features
 *
 * Read's the list features of the OTS service represented by @p adapter.
 *
 * @param adapter otslib adapter
 * @param[out] features @p OTSLIB_LIST_FEATURE_ bitmask of list features
 * @retval -EINVAL if the input parameters are NULL
 * @retval 0 when successful
 * @return other negative values for other errors
 */
int otslib_list_features(void *adapter, unsigned long *features);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_H__ */
