#include <Windows.h>
#include "distormx.h"
#include "api.h"
#include "CommunicationStruct.h"
#include "MonitorContext.h"
#include "context.h"
#include "setFileAttributes_hook.h"
#include "writeFile_hook.h"
#include "writeFileEx_hook.h"
#include "regOpenKeyExW_hook.h"
#include "regCloseKey_hook.h"
#include "regSetValueExW_hook.h"
#include "duplicateHandle_hook.h"
#include "pid.h"

#include "setHooks.h"
MONITORLIBRARY_API DWORD setHooks(void *arg) {
	context.comm_struct = * (struct CommunicationStruct *) arg;
	pid = GetCurrentProcessId();
	context.setFileAttributes_original = (void*)&SetFileAttributesW;
	distormx_hook((void *) & context.setFileAttributes_original, &setFileAttributes_hook);
	context.writeFile_original = (void*)&WriteFile;
	distormx_hook((void *) & context.writeFile_original, &writeFile_hook);
	context.writeFileEx_original = (void*)&WriteFileEx;
	distormx_hook((void *) & context.writeFileEx_original, &writeFileEx_hook);
	context.regOpenKeyExW_original = (void*)&RegOpenKeyExW;
	distormx_hook((void *) &context.regOpenKeyExW_original, &regOpenKeyExW_hook);
	context.regSetValueExW_original = (void*)&RegSetValueExW;
	distormx_hook((void*)&context.regSetValueExW_original, &regSetValueExW_hook);
	context.regCloseKey_original = (void*)&RegCloseKey;
	distormx_hook((void*)&context.regCloseKey_original, &regCloseKey_hook);
	context.duplicateHandle_original = (void*)&DuplicateHandle;
	distormx_hook((void*)&context.duplicateHandle_original, &duplicateHandle_hook);
	return 0;
};