#ifndef _UTILITY_MAXVAL_H_
#define _UTILITY_MAXVAL_H_

#include <climits>
#include "../minimemcached/Entry.h"

template<typename T>
struct MaxValHelper {
};

template<>
struct MaxValHelper<int> {
	static const int value = INT_MAX;
};

template<>
struct MaxValHelper<Entry> {
	static const Entry value;
};

template<typename T>
struct MaxVal {
	static const T value = MaxValHelper<T>::value;
};

#endif /* _UTILITY_MAXVAL_H_ */
