#include <Windows.h>
#include <assert.h>
#include "..\..\StaticLibAVL\avl.h"
#include "StringPoolItem.h"
#include "OpenedRegKey.h"

#include "addTokensToRegistryKey.h"
void addTokensToRegistryKey(struct avl_table* pool, struct OpenedRegKey* key, LPWSTR tokens) {
	wchar_t* context = NULL, * token;
	const wchar_t delimiter[] = L"\\";
	token = wcstok_s(tokens, delimiter, &context);
	while (token) {
		struct StringPoolItem spi_stub = { .item = token }, * found;
		found = avl_find(pool, &spi_stub);
		if (NULL == found) {
			struct StringPoolItem* new_item;
			new_item = malloc(sizeof * new_item);
			new_item->count = 1;
			new_item->item = _wcsdup(token);
			void** probe;
			probe = avl_probe(pool, new_item);
			assert((*probe) == new_item);
			found = *probe;
		}
		else {
			found->count++;
		};
		key->str_items_count++;
		key->str_items = realloc(key->str_items, key->str_items_count * sizeof(*key->str_items));
		key->str_items[key->str_items_count - 1] = found;
		token = wcstok_s(NULL, delimiter, &context);
	};
};