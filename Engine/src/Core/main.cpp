#include "pch.h"
#include <windows.h>

#include "Rendering/DirectX/DXDeviceInstance.h"
#include "Core/Logic/SceneHandler.h"
#include "Core/Logic/Scenes/EditorScene.h"

#include "IO/Input.h"

#include "examples/imgui_impl_win32.h"

// function prototypes
bool initWindow(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height, LPCWSTR title); // Initilise window
void run(HWND hWnd);
void shutdown();

#ifdef ANK_USE_IMGUI
	extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
// Windows message callback 
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef ANK_USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
#endif

	switch (message)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			//g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		}
		return 0;

		case WM_KEYDOWN:
		{
			Input::get().registerKeyDown(wParam, lParam);
		
			if (Input::get().keyPressed(KEY_ESC)) {
				PostQuitMessage(0);
				return 0;
			}

		} break;

		case WM_KEYUP:
		{
			Input::get().registerKeyUp(wParam, lParam);
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

bool setupConsole() 
{
	if (!AllocConsole())
		return false;

	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	if (input == INVALID_HANDLE_VALUE)
		return false;

	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	if (output == INVALID_HANDLE_VALUE)
		return false;

	HANDLE error = GetStdHandle(STD_ERROR_HANDLE);
	if (error == INVALID_HANDLE_VALUE)
		return false;


	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	
	return true;
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#ifdef ANK_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	HWND hWnd;

	if (!setupConsole())
		return 1;

	if (!initWindow(hInstance, nCmdShow, &hWnd, SCREEN_WIDTH * WINDOW_SIZE_FACTOR, SCREEN_HEIGHT * WINDOW_SIZE_FACTOR, L"Sandbox"))
		return 1;

	DXDeviceInstance::get().init(hWnd);
	run(hWnd);
	shutdown();

	return 0;
}

void shutdown() {
	FreeConsole();
}


void run(HWND hWnd)
{
	// Used for delta time
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto prevTime = currentTime;
	double dt = 0;
	unsigned frames = 0;
	double elapsedTime = 0;

	SceneHandler sceneHandler;
	sceneHandler.setCurrentScene(new EditorScene);
	

	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);

			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else
		{

			// Handle DirectX error output messages
#if ANK_DEBUG
			DXDeviceInstance::get().handleErrorMessage();
#endif
			currentTime = std::chrono::high_resolution_clock::now();
			dt = std::chrono::duration<double>(currentTime - prevTime).count();
			prevTime = currentTime;
			elapsedTime += dt;
			frames++;

			if (elapsedTime >= 1.0) {
				SetWindowTextA(hWnd, ("Sandbox (FPS: " + std::to_string(frames) + ", dt: " + std::to_string((elapsedTime / frames) * 1000.f) + " ms)").c_str());
				elapsedTime = 0;
				frames = 0;
			}

			sceneHandler.tick(dt);
		}
	}
}

bool initWindow(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height, LPCWSTR title) {
	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
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
	return true;
}
