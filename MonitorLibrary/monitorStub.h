#pragma once
#ifdef MONITORLIBRARY_EXPORTS
#define MONITORLIBRARY_API __declspec(dllexport)
#else
#define MONITORLIBRARY_API __declspec(dllimport)
#endif

MONITORLIBRARY_API void monitorStub();