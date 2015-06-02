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
#include "util.h"

#include <Windows.h>

LRESULT CALLBACK WindowProcedure
(HWND hwnd, unsigned int message, WPARAM wParam, LPARAM lParam);

class WinClass
{
public:
	WinClass(WNDPROC winProc, char const * className, HINSTANCE hInst);
	void Register()
	{
		::RegisterClass(&_class);
	}
private:
	WNDCLASS _class;
};

WinClass::WinClass
(WNDPROC winProc, char const * className, HINSTANCE hInst)
{
	_class.style = 0;
	_class.lpfnWndProc = winProc; // window procedure: mandatory
	_class.cbClsExtra = 0;
	_class.cbWndExtra = 0;
	_class.hInstance = hInst;         // owner of the class: mandatory
	_class.hIcon = 0;
	_class.hCursor = ::LoadCursor(0, IDC_ARROW); // optional
	_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // optional
	_class.lpszMenuName = 0;
	_class.lpszClassName = className; // mandatory
}

class WinMaker
{
public:
	WinMaker() : _hwnd(0) {}
	WinMaker(char const * caption,
		char const * className,
		HINSTANCE hInstance);
	void Show(int cmdShow)
	{
		::ShowWindow(_hwnd, cmdShow);
		::UpdateWindow(_hwnd);
	}
protected:
	HWND _hwnd;
};

WinMaker::WinMaker(char const * caption,
	char const * className,
	HINSTANCE hInstance)
{
	_hwnd = ::CreateWindow(
		className,            // name of a registered window class
		caption,              // window caption
		WS_OVERLAPPEDWINDOW,  // window style
		CW_USEDEFAULT,        // x position
		CW_USEDEFAULT,        // y position
		CW_USEDEFAULT,        // witdh
		CW_USEDEFAULT,        // height
		0,                    // handle to parent window
		0,                    // handle to menu
		hInstance,            // application instance
		0);                   // window creation data
}

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
            char * cmdParam, int cmdShow)
{
    char className [] = "Winnie";

    WinClass winClass (WindowProcedure, className, hInst);
    winClass.Register ();

    WinMaker win ("Hello Windows!", className, hInst);
    win.Show (cmdShow);

    MSG  msg;
    int status;

    while ((status = ::GetMessage (& msg, 0, 0, 0)) != 0)
    {
        if (status == -1)
            return -1;
        ::DispatchMessage (& msg);
    }

    return msg.wParam;
}

// Window Procedure called by Windows
LRESULT CALLBACK WindowProcedure
(HWND hwnd, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	}
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}