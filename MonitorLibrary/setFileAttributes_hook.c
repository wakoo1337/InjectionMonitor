#include <Windows.h>
#include <stdint.h>
#include "CommunicationStruct.h"
#include "MonitorContext.h"
#include "LogMessageStruct.h"
#include "context.h"
#include "functions.h"
#include "pid.h"

#include "setFileAttributes_hook.h"
BOOL WINAPI setFileAttributes_hook(LPCWSTR lpFileName, DWORD dwFileAttributes) {
	WaitForSingleObject(context.comm_struct.mutex, INFINITE);
	struct LogMessageStruct* logmsg;
	HANDLE process_heap = GetProcessHeap();
	logmsg = HeapAlloc(process_heap, 0, (sizeof * logmsg) + sizeof(unsigned int));
	if (logmsg) {
		logmsg->size = sizeof(unsigned int);
		logmsg->pid = pid;
		logmsg->function = FUNCTION_SETFILEATTRIBUTES;
		*((unsigned int*)logmsg->content) = dwFileAttributes;
		DWORD written;
		context.writeFile_original(context.comm_struct.pipe, logmsg, (sizeof * logmsg) + sizeof(unsigned int), &written, NULL);
		ReleaseMutex(context.comm_struct.mutex);
		HeapFree(process_heap, 0, logmsg);
	};
	return context.setFileAttributes_original(lpFileName, dwFileAttributes);
};