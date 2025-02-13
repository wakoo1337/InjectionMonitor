#include <Windows.h>
#include "distormx.h"
#include "MonitorContext.h"
#include "context.h"
#include "setFileAttributes_hook.h"
#include "writeFile_hook.h"
#include "writeFileEx_hook.h"

#include "setHooks.h"
__declspec(dllexport) DWORD setHooks(void *arg) {
	void* sfa = (void*)&SetFileAttributesW;
	distormx_hook(&sfa, &setFileAttributes_hook);
	context.setFileAttributes_original = (BOOL(*)(LPCWSTR, DWORD)) sfa;
	void* wf = (void*)&WriteFile;
	distormx_hook(&wf, &writeFile_hook);
	context.writeFile_original = wf;
	void* wfx = (void*)&WriteFileEx;
	distormx_hook(&wfx, &writeFileEx_hook);
	context.writeFileEx_original = wfx;
	return 0;
};