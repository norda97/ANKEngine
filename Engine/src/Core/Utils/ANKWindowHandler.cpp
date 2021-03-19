#include "pch.h"
#include "ANKWindowHandler.h"

#ifdef WIN32
#include "examples/imgui_impl_win32.h"
#endif

#include "Core/IO/Input.h"
#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#if ANK_USE_IMGUI
#include "Core/Utils/InterfaceGfxDbg/ANKDebugInterface.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

HWND ANKWindowHandler::s_hWnd;
uint32_t ANKWindowHandler::s_WindowWidth = SCREEN_WIDTH;
uint32_t ANKWindowHandler::s_WindowHeight = SCREEN_HEIGHT;
std::vector<std::function<void(uint32_t, uint32_t)>> ANKWindowHandler::s_ResizeCallbacks;

bool ANKWindowHandler::SetUpWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title)
{
	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ANKWindowHandler::WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	// Retrieve Screen resolution
	const HWND hDesktop = GetDesktopWindow();
	RECT desktop;
	GetWindowRect(hDesktop, &desktop);
	const uint32_t desktopWidth		= desktop.right;
	const uint32_t desktopHeight	= desktop.bottom;
	const uint32_t windowWidth		= wr.right - wr.left;
	const uint32_t windowHeight		= wr.bottom - wr.top;

	s_hWnd = CreateWindowEx(NULL,
		L"WindowClass1",
		title,
		WS_OVERLAPPEDWINDOW,
		(desktopWidth - windowWidth) * 0.5,		// x-position of the window
		(desktopHeight - windowHeight) * 0.5,	// y-position of the window
		windowWidth,							// width of the window
		windowHeight,							// height of the window
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(s_hWnd, nCmdShow);

	return true;
}

LRESULT ANKWindowHandler::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

#if ANK_USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
#endif

	switch (message)
	{
	case WM_SIZE:
	{
		if (wParam != SIZE_MINIMIZED)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			s_WindowWidth = width;
			s_WindowHeight = height;

			HandleResizeCallbacks(width, height);

#if ANK_USE_IMGUI
			ANKDebugInterface::Resize(width, height);
#endif
		}
		return 0;
	}

	case WM_KEYDOWN:
	{
		Input::Get().registerKeyDown(wParam, lParam);

		if (Input::Get().keyPressed(KEY_ESC)) {
			PostQuitMessage(0);
			return 0;
		}

	} break;

	case WM_KEYUP:
	{
		Input::Get().registerKeyUp(wParam, lParam);
	} break;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void ANKWindowHandler::RegisterResizeCallback(std::function<void(uint32_t, uint32_t)> callbackFunc)
{
	s_ResizeCallbacks.push_back(callbackFunc);
}

void ANKWindowHandler::HandleResizeCallbacks(uint32_t width, uint32_t height)
{
	for (auto& resizeFunc : s_ResizeCallbacks)
	{
		resizeFunc(width, height);
	}
}

