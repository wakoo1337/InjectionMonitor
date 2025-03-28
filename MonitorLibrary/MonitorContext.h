#pragma once
struct MonitorContext {
	struct CommunicationStruct comm_struct;
	BOOL(WINAPI* setFileAttributes_original)(LPCWSTR, DWORD);
	BOOL(WINAPI* writeFile_original)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
	BOOL(WINAPI* writeFileEx_original)(HANDLE, LPCVOID, DWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE);
	LSTATUS(*regOpenKeyExW_original)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
	LSTATUS(*regSetValueExW_original)(HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD);
	LSTATUS(*regCloseKey_original)(HKEY);
	BOOL(*duplicateHandle_original)(HANDLE, HANDLE, HANDLE, LPHANDLE, DWORD, BOOL, DWORD);
};