#include "pch.h"
#include "Rendering/DirectX/DXDeviceInstance.h"
#include "Utils/ANKWindowHandler.h"
#include "Core/Logic/SceneHandler.h"

// Scenes
#include "Core/Logic/Scenes/MainScene.h"

#include "Utils/ThreadPool/ANKThreadPool.h"

// function prototypes
//bool initWindow(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height, LPCWSTR title); // Initilise window
void Run();
void Shutdown();

bool SetupConsole() 
{
	if (!AllocConsole())
		return false;

	HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	if (input == INVALID_HANDLE_VALUE)
		return false;

	g_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (g_ConsoleHandle == INVALID_HANDLE_VALUE)
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
	if (!SetupConsole())
	{
		Shutdown();
		return 1;
	}

	if (!ANKWindowHandler::SetUpWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT, L"Sandbox"))
	{
		LOG_ERROR("Failed to set up window!");
		Shutdown();
		return 1;
	}

	DXDeviceInstance::Init(ANKWindowHandler::s_hWnd);

	/* ########## THREAD POOL TEST ################
	
	ANKThreadPool::Init();

	ANKThreadPool::QueueJob([]() {
		for (size_t i = 0; i < 10; i++)
		{
			std::cout << "aaaaaaaaaaaaaaaa" << std::endl;
		}
		});

	ANKThreadPool::QueueJob([]() {
		for (size_t i = 0; i < 10; i++)
		{
			std::cout << "bbbbbbbbbbbbbbbb" << std::endl;
		}
		});

	ANKThreadPool::QueueJob([]() {
		for (size_t i = 0; i < 10; i++)
		{
			std::cout << "ccccccccccccc" << std::endl;
		}
		});*/
	
	Run();
	Shutdown();

	return 0;
}

void Run()
{
	// Used for delta time
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto prevTime = currentTime;
	double dt = 0;
	unsigned frames = 0;
	double elapsedTime = 0;

	ANKThreadPool::Init();

	SceneHandler sceneHandler;
	sceneHandler.SetCurrentScene(new MainScene());

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

		{

			// Handle DirectX error output messages
#if ANK_DEBUG
			DXDeviceInstance::HandleErrorMessage();
#endif
			currentTime = std::chrono::high_resolution_clock::now();
			dt = std::chrono::duration<double>(currentTime - prevTime).count();
			prevTime = currentTime;
			elapsedTime += dt;
			frames++;

			if (elapsedTime >= 1.0) {
				SetWindowTextA(ANKWindowHandler::s_hWnd, ("Sandbox (FPS: " + std::to_string(frames) + ", dt: " + std::to_string((elapsedTime / frames) * 1000.f) + " ms)").c_str());
				elapsedTime = 0;
				frames = 0;
			}
			
			// Updates scene, which in turn updates ECS system of the scene and rendering of MeshInstances
			sceneHandler.Tick(dt);
		}
	}
}

void Shutdown() {
	FreeConsole();
	ANKThreadPool::Release();
	DXDeviceInstance::Release();
}
