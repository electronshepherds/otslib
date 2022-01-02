#ifndef __OTSLIB_LIST_H__
#define __OTSLIB_LIST_H__

/**
 * @brief List
 * @defgroup otslib_list Object Transfer Service List
 * @ingroup otslib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

int otslib_first(void *adapter);
int otslib_last(void *adapter);
int otslib_previous(void *adapter);
int otslib_next(void *adapter);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __OTSLIB_LIST_H__ */
