#include <Windows.h>

#include "writeFile_hook.h"
BOOL WINAPI writeFile_hook(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
	SetLastError(ERROR_ACCESS_DENIED);
	return FALSE;
};