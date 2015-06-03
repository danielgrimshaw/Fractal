#ifndef __GUI_H__
#define __GUI_H__
#include <windows.h>

// window class name
const char g_szClassName[] = "Fractal Settings";

// callback procedure
LRESULT CALLBACK guiProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif