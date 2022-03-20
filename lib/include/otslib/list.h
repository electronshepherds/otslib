#ifndef __OTSLIB_LIST_H__
#define __OTSLIB_LIST_H__

/**
 * @brief List
 * @defgroup otslib_list Object Transfer Service List
 * @ingroup otslib
 * @{
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Select the first object
 *
 * @param adapter an otslib adapter
 * @retval -EINVAL if @p adapter is invalid
 * @retval 0 when successful
 * @retval other negative values for other errors
 */
int otslib_first(void *adapter);

/**
 * @brief Select the last object
 *
 * @param adapter an otslib adapter
 * @retval -EINVAL if @p adapter is invalid
 * @retval 0 when successful
 * @retval other negative values for other errors
 */
int otslib_last(void *adapter);

/**
 * @brief Select the previous object
 *
 * @param adapter an otslib adapter
 * @retval -EINVAL if @p adapter is invalid
 * @retval 0 when successful
 * @retval other negative values for other errors
 */
int otslib_previous(void *adapter);

/**
 * @brief Select the next object
 *
 * @param adapter an otslib adapter
 * @retval -EINVAL if @p adapter is invalid
 * @retval 0 when successful
 * @retval other negative values for other errors
 */
int otslib_next(void *adapter);

/**
 * @brief Select the object with specified ID
 *
 * @param adapter an otslib adapter
 * @param id object ID to select
 * @retval -EINVAL if @p adapter or @p id is invalid
 * @retval 0 when successful
 * @retval other negative values for other errors
 */
int otslib_goto(void *adapter, uint64_t id);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_LIST_H__ */
