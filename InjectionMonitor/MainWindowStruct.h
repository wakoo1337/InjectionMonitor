#pragma once
struct MainWindowStruct {
	unsigned int dpi;
	HWND path_static, path_edit;
	HWND find_button;
	HWND run_button, exit_button, about_button;
	HANDLE find_icon;
	HFONT font;
};