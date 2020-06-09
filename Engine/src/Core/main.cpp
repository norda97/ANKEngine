#include "pch.h"
#include <windows.h>

#include "Rendering/DirectX/DXDeviceInstance.h"
#include "Rendering/DirectX/DXShader.h"
#include "Rendering/DirectX/DXBuffer.h"

#include "Core/Utils/Cameras/Camera.h"

// Windows message callback 
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

// global declarations of COM-objects
DXShader testShader;
DXBuffer vertexBuffer;
DXBuffer constantBuffer;

Camera camera;

struct VERTEX
{
	FLOAT x, y, z;		// position
};

// function prototypes
bool initWindow(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height, LPCWSTR title); // Initilise window
void renderFrame(double dt);

bool initPipeline();
bool initGraphics();

void run(HWND hWnd);
void shutdown();

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#ifdef TS_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	HWND hWnd;


	if (!initWindow(hInstance, nCmdShow, &hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, L"Sandbox"))
		return 1;

	DXDeviceInstance::get().init(hWnd);

	camera.init(XM_PI * 0.25f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), Vector3(0.0f), Vector3(0.f, 0.f, 4.0f), 0.1f, 10.0f);

	if(!initPipeline())
		return 1;
	if(!initGraphics())
		return 1;


	run(hWnd);

	shutdown();

	return 0;
}


void run(HWND hWnd)
{
	// Used for delta time
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto prevTime = currentTime;
	double dt = 0;
	unsigned frames = 0;
	double elapsedTime = 0;

	MSG msg = { 0 };

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);

			// check to see if it's time to quit
			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			
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

			// Run game
			renderFrame(dt);
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

struct alignas(16) SceneVariables
{
	Matrix VP;
	float time;
};

void renderFrame(double dt)
{
	static double elapTime = 0.0;
	elapTime += dt;

	SceneVariables sv = { camera.getViewProjection(), elapTime };
	DXDeviceInstance::get().getDevCon()->UpdateSubresource(constantBuffer.getBuffer(), 0, NULL, (void*)&sv, 0, 0);

	Color vec = Color(0.0f, 0.2f + (1.0f+sinf(elapTime)) * 0.2, 0.4f, 1.0f);
	DXDeviceInstance::get().getDevCon()->ClearRenderTargetView(DXDeviceInstance::get().getBackbuffer(), &vec.x);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	ID3D11Buffer* vBuffers = vertexBuffer.getBuffer();
	ID3D11Buffer* cBuffers = constantBuffer.getBuffer();
	DXDeviceInstance::get().getDevCon()->IASetVertexBuffers(0, 1, &vBuffers, &stride, &offset);
	DXDeviceInstance::get().getDevCon()->VSSetConstantBuffers(0, 1, &cBuffers);

	DXDeviceInstance::get().getDevCon()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXDeviceInstance::get().getDevCon()->Draw(3, 0);
	DXDeviceInstance::get().getSwapchain()->Present(0, 0);
}

void shutdown() {

}

bool initPipeline()
{
	// Create input layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!testShader.init("BasicShader.hlsl", "BasicShader.hlsl", ied))
		return false;
	testShader.prepare();

	return true;
}


bool initGraphics()
{
	// ############## VERTICES ###################
	VERTEX OurVertices[] =
	{
		{0.0f, 0.5f, 0.0f},
		{0.45f, -0.5, 0.0f},
		{-0.45f, -0.5f, 0.0f}
	};

	if (!vertexBuffer.init(OurVertices, sizeof(OurVertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0))
		return false;

	if (!constantBuffer.init(NULL, sizeof(SceneVariables), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0))
		return false;

	return true;
}