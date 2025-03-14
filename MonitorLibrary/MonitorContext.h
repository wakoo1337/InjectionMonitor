#pragma once
struct MonitorContext {
	struct CommunicationStruct comm_struct;
	BOOL(WINAPI *setFileAttributes_original)(LPCWSTR, DWORD);
	BOOL(WINAPI *writeFile_original)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
	BOOL(WINAPI* writeFileEx_original)(HANDLE, LPCVOID, DWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE);
};