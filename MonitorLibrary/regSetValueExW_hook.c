#include <Windows.h>
#include <stdint.h>
#include "CommunicationStruct.h"
#include "MonitorContext.h"
#include "LogMessageStruct.h"
#include "context.h"
#include "pid.h"
#include "functions.h"

#include "regSetValueExW_hook.h"
LSTATUS regSetValueExW_hook(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData) {
	LSTATUS ret;
	ret = context.regSetValueExW_original(hKey, lpValueName, Reserved, dwType, lpData, cbData);
	if (ret == ERROR_SUCCESS) {
		HANDLE heap;
		heap = GetProcessHeap();
		struct LogMessageStruct* lms;
		lms = HeapAlloc(heap, 0, (sizeof * lms) + sizeof(HKEY));
		if (lms) {
			lms->size = sizeof(HKEY);
			lms->pid = pid;
			lms->function = FUNCTION_REGSETVALUEEXW;
			*(HKEY*)lms->content = hKey;
			WaitForSingleObject(context.comm_struct.mutex, INFINITE);
			DWORD written;
			context.writeFile_original(context.comm_struct.pipe, lms, (sizeof * lms) + sizeof(HKEY), &written, NULL);
			ReleaseMutex(context.comm_struct.mutex);
			HeapFree(heap, 0, lms);
		};
	};
	return ret;
};