#pragma once
struct OpenedRegKey {
	unsigned int pid;
	HKEY key;
	unsigned int str_items_count;
	struct StringPoolItem **str_items;
};