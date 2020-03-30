#include "pch.h"
#include "WinWrapper.h"

static bool initilised = false;
static MSG msg = { 0 };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void WinWrapper::init(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height, LPCWSTR title)
{
	if(!initilised) {
		RECT wr = {0, 0, width, height};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszClassName = L"WindowClass1";

		RegisterClassEx(&wc);



		*hWnd = CreateWindowEx(NULL,
			L"WindowClass1",
			title,
			WS_OVERLAPPEDWINDOW,
			300,    // x-position of the window
			300,    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,
			NULL,
			hInstance,
			NULL);

		
		ShowWindow(*hWnd, nCmdShow);
		initilised = true;
	}
}

bool WinWrapper::handleMessage()
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// translate keystroke messages into the right format
		TranslateMessage(&msg);

		// send the message to the WindowProc function
		DispatchMessage(&msg);

		// check to see if it's time to quit
		if (msg.message == WM_QUIT)
			return false;
	}
	else 
	{
		// Run game code
	}
	return true;
}

