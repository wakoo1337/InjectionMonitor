#include <Windows.h>
#include <stdint.h>
#include "CommunicationStruct.h"
#include "MonitorContext.h"
#include "LogMessageStruct.h"
#include "context.h"
#include "functions.h"

#include "setFileAttributes_hook.h"
BOOL WINAPI setFileAttributes_hook(LPCWSTR lpFileName, DWORD dwFileAttributes) {
	WaitForSingleObject(context.comm_struct.mutex, INFINITE);
	struct LogMessageStruct* logmsg;
	logmsg = malloc((sizeof *logmsg) + sizeof(unsigned int));
	logmsg->size = sizeof(unsigned int);
	logmsg->pid = GetCurrentProcessId();
	logmsg->function = FUNCTION_SETFILEATTRIBUTES;
	*((unsigned int *) logmsg->content) = dwFileAttributes;
	ReleaseMutex(context.comm_struct.mutex);
	DWORD written;
	WriteFile(context.comm_struct.pipe, logmsg, (sizeof logmsg) + sizeof(unsigned int), &written, NULL);
	free(logmsg);
	return context.setFileAttributes_original(lpFileName, dwFileAttributes);
};