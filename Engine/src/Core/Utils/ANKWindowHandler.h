#pragma once
#include <windows.h>
#include <functional>

class ANKWindowHandler
{
private:
	ANKWindowHandler() {};
	ANKWindowHandler(const ANKWindowHandler& other) = delete;
	ANKWindowHandler(ANKWindowHandler&& other) = delete;
public:
	static bool SetUpWindow(HINSTANCE hInstance, int nCmdShow, int width, int height, LPCWSTR title);

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void RegisterResizeCallback(std::function<void(uint32_t, uint32_t)> callbackFunc);

	static HWND s_hWnd;

	static uint32_t s_WindowWidth;
	static uint32_t s_WindowHeight;
private:
	inline static void HandleResizeCallbacks(uint32_t width, uint32_t height);

private:
	static std::vector<std::function<void(uint32_t, uint32_t)>> s_ResizeCallbacks;
};