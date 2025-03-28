#include <Windows.h>
#include <stdint.h>
#include "CommunicationStruct.h"
#include "MonitorContext.h"
#include "LogMessageStruct.h"
#include "RegData.h"
#include "context.h"
#include "functions.h"
#include "pid.h"

#include "regOpenKeyExW_hook.h"
LSTATUS regOpenKeyExW_hook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
	LSTATUS ret;
	ret = context.regOpenKeyExW_original(hKey, lpSubKey, ulOptions, samDesired, phkResult);
	if (ret == ERROR_SUCCESS) {
		HANDLE heap;
		heap = GetProcessHeap();
		const unsigned int path_len = wcslen(lpSubKey);
		struct LogMessageStruct* lms;
		const unsigned int lms_size = (sizeof * lms) + sizeof(struct RegData) + ((path_len + 1) * sizeof(WCHAR));
		lms = HeapAlloc(heap, 0, lms_size);
		if (lms) {
			lms->size = sizeof(struct RegData) + ((path_len + 1) * sizeof(WCHAR));
			lms->pid = pid;
			lms->function = FUNCTION_REGOPENKEYEXW;
			struct RegData* rd = (struct RegData*)lms->content;
			rd->old_key = hKey;
			rd->new_key = *phkResult;
			rd->path_len = path_len;
			memcpy(rd->path, lpSubKey, rd->path_len * sizeof(WCHAR));
			rd->path[rd->path_len] = L'\0';
			WaitForSingleObject(context.comm_struct.mutex, INFINITE);
			DWORD written;
			context.writeFile_original(context.comm_struct.pipe, lms, lms_size, &written, NULL);
			ReleaseMutex(context.comm_struct.mutex);
			HeapFree(heap, 0, lms);
		};
	};
	return ret;
};