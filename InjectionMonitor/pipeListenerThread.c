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
		ReadFile(main_struct->read_end, &size, sizeof size, NULL, NULL);
		struct LogMessageStruct* lms;
		lms = malloc(size);
		lms->size = size;
		ReadFile(main_struct->read_end, ((char *) lms) + sizeof(unsigned int), size - sizeof(unsigned int), NULL, NULL);
		free(lms);
	};
	CloseHandle(main_struct->comm_struct.pipe);
	return 0;
};