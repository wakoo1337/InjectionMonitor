#include <Windows.h>
#include <stdint.h>
#include <assert.h>
#include "..\..\StaticLibAVL\avl.h"
#include "..\MonitorLibrary\CommunicationStruct.h"
#include "MainWindowStruct.h"
#include "..\MonitorLibrary\LogMessageStruct.h"
#include "..\MonitorLibrary\functions.h"
#include "..\MonitorLibrary\RegData.h"
#include "StringPoolItem.h"
#include "OpenedRegKey.h"
#include "WM_LOGMESSAGE.h"

#include "pipeListenerThread.h"
DWORD WINAPI pipeListenerThread(LPVOID lpParameter) {
	struct MainWindowStruct* main_struct = (struct MainWindowStruct*)lpParameter;
	for (;;) {
		struct LogMessageStruct lms_hdr;
		DWORD readed;
		if ((!ReadFile(main_struct->read_end, &lms_hdr, sizeof lms_hdr, &readed, NULL)) || (readed != sizeof lms_hdr)) {
			return 1;
		};
		if (lms_hdr.size) {
			struct LogMessageStruct* lms;
			lms = malloc((sizeof * lms) + lms_hdr.size);
			if (NULL == lms) {
				return 1;
			};
			memcpy(lms, &lms_hdr, sizeof * lms);
			if ((!ReadFile(main_struct->read_end, ((char*)lms) + (sizeof * lms), lms->size, &readed, NULL)) || (readed != lms->size)) {
				free(lms);
				return 1;
			};
			SYSTEMTIME st;
			GetSystemTime(&st);
			#define PREFIX_BUFFER 100
			WCHAR prefix_buffer[PREFIX_BUFFER];
			ZeroMemory(prefix_buffer, PREFIX_BUFFER * sizeof(WCHAR));
			prefix_buffer[0] = L'[';
			unsigned int prefix_offset = 1;
			_ultow_s(st.wYear, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L'-';
			prefix_offset++;
			_ultow_s(st.wMonth, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L'-';
			prefix_offset++;
			_ultow_s(st.wDay, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L' ';
			prefix_offset++;
			_ultow_s(st.wHour, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L':';
			prefix_offset++;
			_ultow_s(st.wMinute, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L':';
			prefix_offset++;
			_ultow_s(st.wSecond, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L']';
			prefix_offset++;
			prefix_buffer[prefix_offset] = L' ';
			prefix_offset++;
			prefix_buffer[prefix_offset] = L'[';
			prefix_offset++;
			_ultow_s(lms->pid, prefix_buffer + prefix_offset, PREFIX_BUFFER - prefix_offset, 10);
			while ((prefix_offset < PREFIX_BUFFER) && iswdigit(prefix_buffer[prefix_offset])) prefix_offset++;
			prefix_buffer[prefix_offset] = L']';
			prefix_offset++;
			prefix_buffer[prefix_offset] = L' ';
			prefix_offset++;
			prefix_buffer[prefix_offset] = L'\0';
			switch (lms->function) {
			case FUNCTION_SETFILEATTRIBUTES:
				if (lms->size == sizeof(unsigned int)) {
					const unsigned int arg = * (unsigned int *) (lms->content);
					if (arg & FILE_ATTRIBUTE_HIDDEN) {
						SendMessageW(main_struct->main_window, WM_LOGMESSAGE, prefix_offset, prefix_buffer);
						SendMessageW(main_struct->main_window, WM_LOGMESSAGE, 22, L"Попытка скрыть файл!\r\n");
					};
				}
				break;
				case FUNCTION_REGOPENKEYEXW:
				{
					struct RegData* rd;
					rd = (struct RegData *) lms->content;
					if ((rd->old_key == HKEY_CLASSES_ROOT) || (rd->old_key == HKEY_CURRENT_CONFIG) || (rd->old_key == HKEY_USERS)) {
						// Эти ключи нам пока не интересны
					}
					else if ((rd->old_key == HKEY_CURRENT_USER) || (rd->old_key == HKEY_LOCAL_MACHINE)) {
						// Эти интересны
						struct OpenedRegKey* opened_key;
						opened_key = malloc(sizeof *opened_key); // TODO перенести в отдельную функцию
						opened_key->pid = lms->pid;
						opened_key->key = rd->new_key;
						opened_key->str_items = NULL;
						opened_key->str_items_count = 0;
						wchar_t* context = NULL, *token;
						const wchar_t delimiter[] = L"\\";
						token = wcstok_s(rd->path, delimiter, &context);
						while (token) {
							struct StringPoolItem spi_stub = { .item = token }, *found;
							found = avl_find(main_struct->string_pool, &spi_stub);
							if (NULL == found) {
								struct StringPoolItem* new_item;
								new_item = malloc(sizeof * new_item);
								new_item->count = 1;
								new_item->item = _wcsdup(token);
								void** probe;
								probe = avl_probe(main_struct->string_pool, new_item);
								assert((*probe) == new_item);
								found = *probe;
							} else {
								found->count++;
							};
							opened_key->str_items_count++;
							opened_key->str_items = realloc(opened_key->str_items, opened_key->str_items_count * sizeof(*opened_key->str_items));
							opened_key->str_items[opened_key->str_items_count - 1] = found;
							token = wcstok_s(NULL, delimiter, &context);
						};
						void** probe;
						probe = avl_probe(main_struct->reg_keys, opened_key);
						assert((*probe) == opened_key);
					}
					else {
						// А это -- если родителем является другой открытый ключ
					}
				}
				break;
				case FUNCTION_REGSETVALUEEXW:
				{
					struct OpenedRegKey key_stub, *key;
					key_stub.pid = lms->pid;
					key_stub.key = *(HKEY*)lms->content;
					key = avl_find(main_struct->reg_keys, &key_stub);
					if (key->str_items_count == 5) {
						if (!(_wcsicmp(key->str_items[0]->item, L"software") || _wcsicmp(key->str_items[1]->item, L"microsoft") || _wcsicmp(key->str_items[2]->item, L"windows") || _wcsicmp(key->str_items[3]->item, L"currentversion") || _wcsicmp(key->str_items[4]->item, L"run"))) {
							SendMessageW(main_struct->main_window, WM_LOGMESSAGE, prefix_offset, prefix_buffer);
							SendMessageW(main_struct->main_window, WM_LOGMESSAGE, 24, L"Запись в автозагрузку!\r\n");
						};
					};
				}
				break;
			}
			free(lms);
		};
	};
	return 0;
};