/** GUI.cpp
 * Launcher program for Fractal
 * Runs a GUI for selecting settings.
 */

#include <iostream>
#include <cstdlib>
#include <thread>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include "util.h"
#include "Fractals.h"
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
	wc.style = CS_VREDRAW | CS_HREDRAW;
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

	std::thread fractal(startFractal);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	static RECT rect;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		GetClientRect(hwnd, &rect);
		TranslateMessage(&msg);
		InvalidateRect(hwnd, &rect, true);
		UpdateWindow(hwnd);
		DispatchMessage(&msg);
	}
	return (int)(msg.wParam);
}

LRESULT CALLBACK guiProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	static PCTSTR controls = getControls();
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		{
			MessageBox(hwnd, controls, "Controls",
				MB_OK | MB_ICONINFORMATION);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			MessageBox(hwnd, "Written by Daniel Grimshaw", "Fractal Author",
				MB_OK | MB_ICONINFORMATION);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		exit(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		{
			PCTSTR leftText = getLeftStrings();
			PCTSTR rightText = getRightStrings();
			hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rect);
			DrawText(hdc, leftText, -1, &rect, DT_LEFT);
			DrawText(hdc, rightText, -1, &rect, DT_RIGHT);
			EndPaint(hwnd, &ps);
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
