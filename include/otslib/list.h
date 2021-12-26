#ifndef __OTSLIB_LIST_H__
#define __OTSLIB_LIST_H__

struct otslib_adapter;

int otslib_first(struct otslib_adapter *adapter);
int otslib_last(struct otslib_adapter *adapter);
int otslib_previous(struct otslib_adapter *adapter);
int otslib_next(struct otslib_adapter *adapter);

#endif /* __OTSLIB_LIST_H__ */
