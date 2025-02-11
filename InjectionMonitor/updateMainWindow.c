#include <Windows.h>
#include "MainWindowStruct.h"

#include "updateMainWindow.h"
void updateMainWindow(HWND h, struct MainWindowStruct* main_struct) {
	const unsigned int border = 10;
	const unsigned int pstatic_width = 100;
	const unsigned int find_size = 32;
	const unsigned int button_width = 100;
	const unsigned int button_height = 40;
	RECT win_rect;
	GetWindowRect(h, &win_rect);
	const unsigned int win_width = win_rect.right - win_rect.left;
	const unsigned int win_height = win_rect.bottom - win_rect.top;
	SetWindowPos(main_struct->path_static, NULL,
		MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(pstatic_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(main_struct->find_button, NULL,
		win_width - 3 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) - MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(main_struct->path_edit, NULL,
		2 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) + MulDiv(pstatic_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		win_width - 6 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) - MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) - MulDiv(pstatic_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(main_struct->run_button, NULL,
		win_width - 3 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) - MulDiv(button_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		2 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) + MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(button_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(button_height, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(main_struct->exit_button, NULL,
		win_width - 4 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) - 2 * MulDiv(button_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		2 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) + MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(button_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(button_height, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(main_struct->about_button, NULL,
		MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		2 * MulDiv(border, main_struct->dpi, USER_DEFAULT_SCREEN_DPI) + MulDiv(find_size, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(button_width, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		MulDiv(button_height, main_struct->dpi, USER_DEFAULT_SCREEN_DPI),
		SWP_NOZORDER | SWP_NOACTIVATE);
};