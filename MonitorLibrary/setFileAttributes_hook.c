#include <Windows.h>
#include "MonitorContext.h"
#include "context.h"

#include "setFileAttributes_hook.h"
BOOL WINAPI setFileAttributes_hook(LPCWSTR lpFileName, DWORD dwFileAttributes) {
	return context.setFileAttributes_original(lpFileName, dwFileAttributes);
};