#include <Windows.h>
#include <shobjidl.h>
#include "resource.h"
#include "..\MonitorLibrary\CommunicationStruct.h"
#include "MainWindowStruct.h"
#include "updateMainWindow.h"
#include "aboutDialog.h"
#include "pipeListenerThread.h"
#include "WM_LOGMESSAGE.h"

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
		main_struct->msg_length = 19;
		main_struct->main_window = h;
		if (!CreatePipe(&main_struct->read_end, &main_struct->comm_struct.pipe, NULL, 0)) {
			return -1;
		};
		main_struct->listener_thread = CreateThread(NULL, 0, &pipeListenerThread, main_struct, 0, NULL);
		main_struct->path_static = CreateWindowExW(0, L"static", L"Путь", WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, h, NULL, main_struct->hInstance, NULL);
		main_struct->find_button = CreateWindowExW(0, L"button", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 0, 0, 0, 0, h, (HMENU)COMMAND_FIND, main_struct->hInstance, NULL);
		SHSTOCKICONINFO find_info;
		find_info.cbSize = sizeof find_info;
		SHGetStockIconInfo(SIID_FIND, SHGSI_ICON, &find_info);
		main_struct->find_icon = find_info.hIcon;
		PostMessageW(main_struct->find_button, BM_SETIMAGE, IMAGE_ICON, (LPARAM)main_struct->find_icon);
		main_struct->path_edit = CreateWindowExW(0, L"edit", L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, h, NULL, main_struct->hInstance, NULL);
		main_struct->run_button = CreateWindowExW(0, L"button", L"Запустить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_RUN, main_struct->hInstance, NULL);
		main_struct->exit_button = CreateWindowExW(0, L"button", L"Выйти", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_EXIT, main_struct->hInstance, NULL);
		main_struct->about_button = CreateWindowExW(0, L"button", L"О программе", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_ABOUT, main_struct->hInstance, NULL);
		main_struct->messages_edit = CreateWindowExW(0, L"edit", L"Injection Monitor", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY, 0, 0, 0, 0, h, 0, main_struct->hInstance, NULL);
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
		PostMessageW(main_struct->messages_edit, WM_SETFONT, (WPARAM)main_struct->font, TRUE);
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
		DestroyWindow(main_struct->messages_edit);
		DestroyIcon(main_struct->find_icon);
		DeleteObject(main_struct->font);
		PostQuitMessage(0);
	}
	break;
	case WM_COMMAND:
	{
		const unsigned int cmd = LOWORD(w);
		if (cmd == COMMAND_FIND) {
			IFileOpenDialog* dialog;
			if (FAILED(CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog, &dialog))) {
				DestroyWindow(h);
				return 1;
			};
			static const COMDLG_FILTERSPEC filter[] = {
				{
					.pszName = L"Исполняемые файлы",
					.pszSpec = L"*.exe;*.cpl;*.ocx;*.com;*.pif"
				},{
					.pszName = L"Все файлы",
					.pszSpec = L"*.*"
				}
			};
			if (FAILED(dialog->lpVtbl->SetFileTypes(dialog, 2, filter))) {
				dialog->lpVtbl->Release(dialog);
				DestroyWindow(h);
				return 1;
			};
			if (FAILED(dialog->lpVtbl->SetOptions(dialog, FOS_NOCHANGEDIR | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST))) {
				dialog->lpVtbl->Release(dialog);
				DestroyWindow(h);
				return 1;
			};
			if (FAILED(dialog->lpVtbl->SetTitle(dialog, L"Выбор файла"))) {
				dialog->lpVtbl->Release(dialog);
				DestroyWindow(h);
				return 1;
			};
			HRESULT show_result;
			show_result = dialog->lpVtbl->Show(dialog, h);
			if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == show_result) {
				dialog->lpVtbl->Release(dialog);
				return 0;
			};
			if (FAILED(show_result)) {
				dialog->lpVtbl->Release(dialog);
				DestroyWindow(h);
				return 1;
			};
			IShellItem* result;
			if (FAILED(dialog->lpVtbl->GetResult(dialog, &result))) {
				dialog->lpVtbl->Release(dialog);
				DestroyWindow(h);
				return 1;
			};
			LPWSTR path;
			if (FAILED(result->lpVtbl->GetDisplayName(result, SIGDN_FILESYSPATH, &path))) {
				result->lpVtbl->Release(result);
				dialog->lpVtbl->Release(dialog);
				DestroyWindow(h);
				return 1;
			};
			SetWindowTextW(main_struct->path_edit, path);
			CoTaskMemFree(path);
			result->lpVtbl->Release(result);
			dialog->lpVtbl->Release(dialog);
		}
		else if (cmd == COMMAND_RUN) {
			const int length = GetWindowTextLengthW(main_struct->path_edit);
			if (0 == length) {
				MessageBoxW(h, L"Укажите путь к исполняемому файлу", L"", MB_OK | MB_ICONERROR);
				return 0;
			};
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
			unsigned int path_size;
			path_size = GetFullPathNameW(L".\\MonitorLibrary.dll", 0, NULL, NULL);
			if (0 == path_size) {
				free(path);
				DestroyWindow(h);
				return 1;
			};
			LPWSTR path_buffer, file_part;
			path_buffer = malloc(path_size * sizeof(wchar_t));
			if (NULL == path_buffer) {
				free(path);
				DestroyWindow(h);
				return 1;
			};
			DWORD path_reallen;
			path_reallen = GetFullPathNameW(L".\\MonitorLibrary.dll", path_size, path_buffer, &file_part);
			if ((path_reallen > path_size) || (path_reallen == 0)) {
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			STARTUPINFOW siw = {
				.cb = sizeof(STARTUPINFOW)
			};
			PROCESS_INFORMATION pi;
			if (!CreateProcessW(path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &siw, &pi)) {
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			LPVOID* path_copy;
			path_copy = VirtualAllocEx(pi.hProcess, NULL, path_size * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (NULL == path_copy) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			if (!WriteProcessMemory(pi.hProcess, path_copy, path_buffer, path_size * sizeof(wchar_t), NULL)) {
				VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			HANDLE load_thread;
			load_thread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, path_copy, 0, NULL);
			if (NULL == load_thread) {
				VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			if (WaitForSingleObject(load_thread, INFINITE) == WAIT_FAILED) {
				CloseHandle(load_thread);
				VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			CloseHandle(load_thread);
			if (!VirtualFreeEx(pi.hProcess, path_copy, 0, MEM_RELEASE)) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			HMODULE monitor = GetModuleHandleW(L"MonitorLibrary.dll");
			if (NULL == monitor) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			FARPROC set_hooks;
			set_hooks = GetProcAddress(monitor, "setHooks");
			if (NULL == set_hooks) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			struct CommuncicationStruct* remote_cs;
			remote_cs = VirtualAllocEx(pi.hProcess, NULL, sizeof main_struct->comm_struct, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (NULL == remote_cs) {
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			if (!WriteProcessMemory(pi.hProcess, remote_cs, &main_struct->comm_struct, sizeof main_struct->comm_struct, NULL)) {
				VirtualFreeEx(pi.hProcess, remote_cs, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			HANDLE hook_thread;
			hook_thread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)set_hooks, remote_cs, 0, NULL);
			if (NULL == hook_thread) {
				VirtualFreeEx(pi.hProcess, remote_cs, 0, MEM_RELEASE);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			if (WaitForSingleObject(hook_thread, INFINITE) == WAIT_FAILED) {
				CloseHandle(hook_thread);
				TerminateProcess(pi.hProcess, 0);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				free(path_buffer);
				free(path);
				DestroyWindow(h);
				return 1;
			};
			VirtualFreeEx(pi.hProcess, remote_cs, 0, MEM_RELEASE);
			CloseHandle(hook_thread);
			ResumeThread(pi.hThread);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			free(path_buffer);
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
	case WM_DPICHANGED:
	{
		main_struct->dpi = GetDpiForWindow(h);
		updateMainWindow(h, main_struct);
	}
	case WM_SIZE:
	{
		updateMainWindow(h, main_struct);
	}
	break;
	case WM_CTLCOLORSTATIC:
	{
		if (((HWND)l) == main_struct->messages_edit) {
			LOGBRUSH lbr;
			const HBRUSH lt_brush = GetStockObject(LTGRAY_BRUSH);
			GetObjectW(lt_brush, sizeof lbr, &lbr);
			const COLORREF ltgray = lbr.lbColor;
			const HBRUSH dk_brush = GetStockObject(DKGRAY_BRUSH);
			GetObjectW(dk_brush, sizeof lbr, &lbr);
			const COLORREF dkgray = lbr.lbColor;
			SetTextColor((HDC)w, ltgray);
			SetBkColor((HDC)w, dkgray);
			return (LRESULT)dk_brush;
		}
		else return 0;
	}
	break;
	case WM_LOGMESSAGE:
	{
		const LPWSTR message = (const LPWSTR)l;
		HLOCAL edit_local;
		edit_local = (HLOCAL)SendMessageW(main_struct->messages_edit, EM_GETHANDLE, 0, 0);
		edit_local = LocalReAlloc(edit_local, (main_struct->msg_length + w + 1) * sizeof(wchar_t), 0);
		LPWSTR buffer;
		buffer = LocalLock(edit_local);
		memcpy(buffer + main_struct->msg_length, message, w * sizeof(wchar_t));
		buffer[main_struct->msg_length + w] = L'\0';
		main_struct->msg_length += w;
		LocalUnlock(edit_local);
		SendMessageW(main_struct->messages_edit, EM_SETHANDLE, (WPARAM)edit_local, 0);
	}
	break;
	default:
		return DefWindowProcW(h, u, w, l);
	};
	return 0;
};