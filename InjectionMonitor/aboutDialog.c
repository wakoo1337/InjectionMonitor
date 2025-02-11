#include <Windows.h>

#include "aboutDialog.h"
INT_PTR aboutDialog(HWND h, UINT u, WPARAM w, LPARAM l) {
	switch (u) {
	case WM_COMMAND:
	case WM_CLOSE:
	{
		EndDialog(h, 1);
		return TRUE;
	}
		break;
	};
	return FALSE;
};