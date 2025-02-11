#include <Windows.h>
#include "MainWindowStruct.h"
#include "updateMainWindow.h"

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
		main_struct->dpi = GetDpiForWindow(h);
		main_struct->path_static = CreateWindowExW(0, L"static", L"����", WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, h, NULL, cs->hInstance, NULL);
		main_struct->find_button = CreateWindowExW(0, L"button", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 0, 0, 0, 0, h, (HMENU)COMMAND_FIND, cs->hInstance, NULL);
		SHSTOCKICONINFO find_info;
		find_info.cbSize = sizeof find_info;
		SHGetStockIconInfo(SIID_FIND, SHGSI_ICON, &find_info);
		main_struct->find_icon = find_info.hIcon;
		PostMessageW(main_struct->find_button, BM_SETIMAGE, IMAGE_ICON, main_struct->find_icon);
		main_struct->path_edit = CreateWindowExW(0, L"edit", L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, h, NULL, cs->hInstance, NULL);
		main_struct->run_button = CreateWindowExW(0, L"button", L"���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_RUN, cs->hInstance, NULL);
		main_struct->exit_button = CreateWindowExW(0, L"button", L"�����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_EXIT, cs->hInstance, NULL);
		main_struct->about_button = CreateWindowExW(0, L"button", L"� ���������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, h, (HMENU)COMMAND_ABOUT, cs->hInstance, NULL);
		updateMainWindow(h, main_struct);
		NONCLIENTMETRICSW ncm;
		ncm.cbSize = sizeof ncm;
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
		main_struct->font = CreateFontIndirectW(&ncm.lfMenuFont);
		PostMessageW(main_struct->path_static, WM_SETFONT, main_struct->font, TRUE);
		PostMessageW(main_struct->path_edit, WM_SETFONT, main_struct->font, TRUE);
		PostMessageW(main_struct->run_button, WM_SETFONT, main_struct->font, TRUE);
		PostMessageW(main_struct->exit_button, WM_SETFONT, main_struct->font, TRUE);
		PostMessageW(main_struct->about_button, WM_SETFONT, main_struct->font, TRUE);
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

		}
		else if (cmd == COMMAND_EXIT) {
			DestroyWindow(h);
		}
		else if (cmd == COMMAND_ABOUT) {

		};
	}
	break;
	default:
		return DefWindowProcW(h, u, w, l);
	};
	return 0;
};