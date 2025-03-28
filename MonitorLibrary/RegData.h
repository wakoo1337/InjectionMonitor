#pragma once
struct RegData {
	HKEY old_key;
	HKEY new_key;
	unsigned int path_len;
	WCHAR path[];
};