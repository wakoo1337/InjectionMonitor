#include <Windows.h>
#include "resource.h"
#include "MainWindowStruct.h"
#include "updateMainWindow.h"
#include "aboutDialog.h"

#include "mainWndProc.h"
LRESULT CALLBACK mainWndProc(HWND h, UINT u, WPARAM w, LPARAM l) {
#define COMMAND_FIND 101
#define COMMAND_RUN 102
#define COMMAND_EXIT 103
#define COMMAND_ABOUT 104
	struct MainWindowStruct* main_struct;
	main_struct = (struct MainWindowStruct*)GetWindowLongPtrW(h, GWLP_USERDATA);
	switch (u) {
	case WM_CREATE:
	{
		CREATESTRUCT* cs;
		cs = (CREATESTRUCT*)l;
		main_struct = (struct MainWindowStruct*)cs->lpCreateParams;
		SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)main_struct);
		main_struct->hInstance = cs->hInstance;
		main_struct->dpi = GetDpiForWindow(h);
		main_struct->path_static = CreateWindowExW(0, L"static", L"����", WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, h, NULL, main_struct->hInstance, NULL);
		main_struct->find_button = CreateWindowExW(0, L"button", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 0, 0, 0, 0, h, (HMENU)COMMAND_FIND, main_struct->hInstance, NULL);
		SHSTOCKICONINFO find_info;
		find_info.cbSize = sizeof find_info;
		SHGetStockIconInfo(SIID_FIND, SHGSI_ICON, &find_info);
		main_struct->find_icon = find_info.hIcon;
		PostMessageW(main_struct->find_button, BM_SETIMAGE, IMAGE_ICON, (LPARAM)main_struct->find_icon);
		main_struct->path_edit = CreateWindowExW(0, L"edit", L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, h, NULL, main_struct->hInstance, NULL);
		main_struct->run_button = CreateWindowExW(0, L"button", L"���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_RUN, main_struct->hInstance, NULL);
		main_struct->exit_button = CreateWindowExW(0, L"button", L"�����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_EXIT, main_struct->hInstance, NULL);
		main_struct->about_button = CreateWindowExW(0, L"button", L"� ���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_ABOUT, main_struct->hInstance, NULL);
		updateMainWindow(h, main_struct);
		NONCLIENTMETRICSW ncm;
		ncm.cbSize = sizeof ncm;
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
		main_struct->font = CreateFontIndirectW(&ncm.lfMenuFont);
		PostMessageW(main_struct->path_static, WM_SETFONT, (WPARAM)main_struct->font, TRUE);
		PostMessageW(main_struct->path_edit, WM_SETFONT, (WPARAM)main_struct->font, TRUE);
		PostMessageW(main_struct->run_button, WM_SETFONT, (WPARAM)main_struct->font, TRUE);
		PostMessageW(main_struct->exit_button, WM_SETFONT, (WPARAM)main_struct->font, TRUE);
		PostMessageW(main_struct->about_button, WM_SETFONT, (WPARAM)main_struct->font, TRUE);
		InvalidateRect(h, NULL, TRUE);
	}
	break;
	case WM_CLOSE:
	{
		DestroyWindow(h);
	}
	break;
	case WM_DESTROY:
	{
		DestroyWindow(main_struct->path_static);
		DestroyWindow(main_struct->path_edit);
		DestroyWindow(main_struct->find_button);
		DestroyWindow(main_struct->run_button);
		DestroyWindow(main_struct->exit_button);
		DestroyWindow(main_struct->about_button);
		DestroyIcon(main_struct->find_icon);
		DeleteObject(main_struct->font);
		PostQuitMessage(0);
	}
	break;
	case WM_COMMAND:
	{
		const unsigned int cmd = LOWORD(w);
		if (cmd == COMMAND_FIND) {
		}
		else if (cmd == COMMAND_RUN) {
			const int length = GetWindowTextLengthW(main_struct->path_edit);
			LPWSTR path = malloc(sizeof(wchar_t) * (length + 1));
			if (NULL == path) {
				DestroyWindow(h);
				return 1;
			};
			if (GetWindowTextW(main_struct->path_edit, path, length + 1) != length) {
				free(path);
				DestroyWindow(h);
				return 1;
			};
			FARPROC load_library;
			HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
			if (NULL == kernel32) {
				free(path);
				DestroyWindow(h);
				return 1;
			};
			load_library = GetProcAddress(kernel32, "LoadLibraryW");
			if (NULL == load_library) {
				free(path);
				DestroyWindow(h);
				return 1;
			};
			STARTUPINFOW siw = {
				.cb = sizeof(STARTUPINFOW)
			};
			PROCESS_INFORMATION pi;
			if (!CreateProcessW(path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &siw, &pi)) {
				free(path);
				DestroyWindow(h);
				return 1;
			};
			const wchar_t dll_name[] = L"C:\\Users\\wakoo\\Documents\\�����\\4 �������\\InjectionMonitor\\x64\\Debug\\MonitorLibrary.dll";
			LPVOID* path_copy;
			path_copy = VirtualAllocEx(pi.hProcess, NULL, sizeof dll_name, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (NULL == path_copy) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			if (!WriteProcessMemory(pi.hProcess, path_copy, dll_name, sizeof dll_name, NULL)) {
				VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			HANDLE created_thread;
			created_thread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) load_library, path_copy, 0, NULL);
			if (NULL == created_thread) {
				VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			if (WaitForSingleObject(created_thread, INFINITE) == WAIT_FAILED) {
				CloseHandle(created_thread);
				VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			CloseHandle(created_thread);
			if (!VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE)) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			ResumeThread(pi.hThread);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			free(path);
		}
		else if (cmd == COMMAND_EXIT) {
			DestroyWindow(h);
		}
		else if (cmd == COMMAND_ABOUT) {
			DialogBoxW(main_struct->hInstance, MAKEINTRESOURCEW(IDD_ABOUT), h, &aboutDialog);
		};
	}
	break;
	default:
		return DefWindowProcW(h, u, w, l);
	};
	return 0;
};