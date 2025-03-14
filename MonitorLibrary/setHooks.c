#include <Windows.h>
#include "distormx.h"
#include "api.h"
#include "CommunicationStruct.h"
#include "MonitorContext.h"
#include "context.h"
#include "setFileAttributes_hook.h"
#include "writeFile_hook.h"
#include "writeFileEx_hook.h"

#include "setHooks.h"
MONITORLIBRARY_API DWORD setHooks(void *arg) {
	context.comm_struct = * (struct CommunicationStruct *) arg;
	context.setFileAttributes_original = (void*)&SetFileAttributesW;
	distormx_hook((void *) & context.setFileAttributes_original, &setFileAttributes_hook);
	context.writeFile_original = (void*)&WriteFile;
	distormx_hook((void *) & context.writeFile_original, &writeFile_hook);
	context.writeFileEx_original = (void*)&WriteFileEx;
	distormx_hook((void *) & context.writeFileEx_original, &writeFileEx_hook);
	return 0;
};