#include <Windows.h>
#include "StringPoolItem.h"

#include "stringPoolItemComparator.h"
int stringPoolItemComparator(const void* a_void, const void* b_void, void* futile) {
	const struct StringPoolItem* a = (const struct StringPoolItem*)a_void;
	const struct StringPoolItem* b = (const struct StringPoolItem*)b_void;
	return _wcsicmp(a->item, b->item);
};