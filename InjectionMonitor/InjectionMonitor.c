#include <Windows.h>
#include <commctrl.h>
#include "..\MonitorLibrary\api.h"
#include "..\MonitorLibrary\monitorStub.h"
#include "..\MonitorLibrary\CommunicationStruct.h"
#include "resource.h"
#include "MainWindowStruct.h"
#include "main_class_name.h"
#include "mainWndProc.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow) {
	monitorStub(); // Эта функция ничего не делает, и её нужно вызывать только для того, чтобы библиотека загрузилась в наше адресное пространство
	if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK) return 1;
	static const INITCOMMONCONTROLSEX commctl_init = {
		.dwSize = sizeof(INITCOMMONCONTROLSEX),
		.dwICC = ICC_STANDARD_CLASSES
	};
	InitCommonControlsEx(&commctl_init);
	static WNDCLASSEXW main_class = {
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_VREDRAW | CS_HREDRAW,
		.lpfnWndProc = &mainWndProc,
		.cbClsExtra = 0,
		.cbWndExtra = sizeof(void *),
		.lpszMenuName = NULL,
		.hIconSm = NULL
	};
	main_class.hInstance = hInstance;
	main_class.hCursor = LoadImageW(hInstance, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
	main_class.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	main_class.lpszClassName = main_class_name;
	main_class.hIcon = LoadImageW(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
	RegisterClassExW(&main_class);
	const unsigned int dpi = GetDpiForSystem();
	HWND main_window;
	main_window = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, main_class_name, L"InjectionMonitor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		MulDiv(480, dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(400, dpi, USER_DEFAULT_SCREEN_DPI),
		NULL, NULL, hInstance, malloc(sizeof(struct MainWindowStruct)));
	ShowWindow(main_window, nCmdShow);
	UpdateWindow(main_window);
	MSG msg;
	BOOL gm_ret;
	while ((gm_ret = GetMessageW(&msg, NULL, 0, 0)), ((gm_ret != 0) && (gm_ret != -1))) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	};
	CoUninitialize();
	return 0;
};