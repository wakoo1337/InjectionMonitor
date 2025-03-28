#include <Windows.h>

#include "duplicateHandle_hook.h"
BOOL duplicateHandle_hook(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, LPHANDLE lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD    dwOptions) {
};