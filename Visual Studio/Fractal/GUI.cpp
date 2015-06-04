/** GUI.cpp
 * Launcher program for Fractal
 * Runs a GUI for selecting settings.
 */

#include <iostream>
#include <cstdlib>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
//#include "util.h"
#include "GUI.h"

#include <windows.h>

/**
 * main
 * HINSTANCE hInstance: current executable program
 * HINSTANCE hPrevInstance: NULL for modern windows (used in Win16)
 * LPSTR lpCmdLine: args without command name
 * int nCmdShow: integer to be passed to ShowWindow()
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// Window class
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = guiProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Fractal",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK guiProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		{
			MessageBox(hwnd, "Written by Daniel Grimshaw", "Fractal",
				MB_OK | MB_ICONINFORMATION);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);
			LPRECT r = NULL;
			GetClientRect(hwnd, r);
			char *cpaText[] = {
				"Hello World!",
				"This is a hello world application made in the Win32 API",
				"This example was made by some random dude, aka -LeetGamer-"
			};
			int iY = 5;
			for (int i = 0; i < 3; i++) {
				DrawText(hDC, cpaText[i], -1, r, DT_LEFT);
			}
			FillRect(hDC, r, (HBRUSH)GetStockObject(GRAY_BRUSH));
			EndPaint(hwnd, &ps);
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
