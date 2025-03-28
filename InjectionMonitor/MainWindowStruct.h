#pragma once
struct MainWindowStruct {
	HINSTANCE hInstance;
	unsigned int dpi, msg_length;
	HWND main_window;
	HWND path_static, path_edit;
	HWND find_button;
	HWND run_button, exit_button, about_button;
	HWND messages_edit;
	HANDLE find_icon;
	HFONT font;
	HANDLE listener_thread;
	HANDLE read_end;
	struct CommunicationStruct comm_struct;
	struct avl_table* string_pool;
	struct avl_table* reg_keys;
};