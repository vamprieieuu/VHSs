#ifndef _MOCK_WINDOWS_H
#define _MOCK_WINDOWS_H
#include <stdint.h>
#include <stddef.h>
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint32_t ULONG;
typedef void* HANDLE;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HWND;
typedef void* HDC;
typedef void* HICON;
typedef void* HCURSOR;
typedef void* HBRUSH;
typedef int BOOL;
typedef int PF_Fixed;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define WINAPI
#define APIENTRY
#define CALLBACK
#endif
