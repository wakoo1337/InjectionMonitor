#include <Windows.h>
#include <stdint.h>
#include "..\MonitorLibrary\CommunicationStruct.h"
#include "MainWindowStruct.h"
#include "..\MonitorLibrary\LogMessageStruct.h"
#include "..\MonitorLibrary\functions.h"
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
						static LPCWSTR alert = L"Тревога!\r\n";
						SendMessageW(main_struct->main_window, WM_LOGMESSAGE, prefix_offset, prefix_buffer);
						SendMessageW(main_struct->main_window, WM_LOGMESSAGE, 22, L"Попытка скрыть файл!\r\n");
					};
				}
				break;
			}
			free(lms);
		};
	};
	return 0;
};