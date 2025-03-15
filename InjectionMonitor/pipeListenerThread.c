#include <Windows.h>
#include <stdint.h>
#include "..\MonitorLibrary\CommunicationStruct.h"
#include "MainWindowStruct.h"
#include "..\MonitorLibrary\LogMessageStruct.h"
#include "WM_LOGMESSAGE.h"

#include "pipeListenerThread.h"
DWORD WINAPI pipeListenerThread(LPVOID lpParameter) {
	struct MainWindowStruct* main_struct = (struct MainWindowStruct*) lpParameter;
	for (;;) {
		unsigned int size;
		DWORD readed;
		if ((!ReadFile(main_struct->read_end, &size, sizeof size, &readed, NULL)) || (readed != sizeof size)) {
			return 1;
		};
		struct LogMessageStruct* lms;
		lms = malloc((sizeof *lms) + size);
		if (NULL == lms) {
			return 1;
		};
		lms->size = size;
		if ((!ReadFile(main_struct->read_end, ((char*)lms) + (sizeof *lms), size, &readed, NULL)) || (readed != size)) {
			return 1;
		};
		free(lms);
	};
	CloseHandle(main_struct->comm_struct.pipe);
	return 0;
};