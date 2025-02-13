#include <Windows.h>

#include "writeFileEx_hook.h"
BOOL WINAPI writeFileEx_hook(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
	SetLastError(ERROR_ACCESS_DENIED);
	return FALSE;
};