#include <Windows.h>
#include "OpenedRegKey.h"

#include "openedRegKeyComparator.h"
int openedRegKeyComparator(const void* a_void, const void* b_void, void* futile) {
	const struct OpenedRegKey* a = (const struct OpenedRegKey*) a_void;
	const struct OpenedRegKey* b = (const struct OpenedRegKey*)b_void;

	if (a->pid == b->pid) {
		if (a->key == b->key) return 0;
		else if (a->key > b->key) return 1;
		else return -1;
	}
	else if (a->pid > b->pid) {
		return 1;
	}
	else {
		return -1;
	};
};