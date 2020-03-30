#pragma once
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

namespace WinWrapper
{
	void init(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height, LPCWSTR title);
	bool handleMessage();
}





