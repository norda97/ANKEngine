#include "pch.h"
#include <windows.h>

#include "Rendering/DirectX/DXDeviceInstance.h"
#include "Rendering/DirectX/DXShader.h"
#include "Rendering/DirectX/DXBuffer.h"

#include "Core/Utils/Cameras/Camera.h"

#include "IO/Input.h"

//#include "Core/Rendering/DirectX/DXRenderer.h"

// global declarations of COM-objects
DXShader testShader;
DXShader instanceShader;

DXBuffer vertexBuffer;
DXBuffer indexBuffer;
DXBuffer constantBuffer;
DXBuffer instanceBuffer;


ID3D11RasterizerState* rsNormal = nullptr;
ID3D11RasterizerState* rsWireframe = nullptr;

struct alignas(16) SceneVariables
{
	Matrix VP;
	float time;
};

struct Instance
{
	Matrix position;
	Color color;
};


#define INSTANCE_COUNT 10
Instance instances[INSTANCE_COUNT];

Camera camera;


// Windows message callback 
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

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
#ifdef MINI_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	HWND hWnd;

	if (!setupConsole())
		return 1;

	if (!initWindow(hInstance, nCmdShow, &hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, L"Sandbox"))
		return 1;

	DXDeviceInstance::get().init(hWnd);
	camera.init(XM_PI * 0.25f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), Vector3(0.0f), Vector3(0.f, 0.f, -4.0f), 0.1f, 1000.0f);

	if(!initPipeline())
		return 1;
	if(!initGraphics())
		return 1;
	run(hWnd);
	shutdown();

	return 0;
}

void shutdown() {
	if (rsNormal)
		rsNormal->Release();
	if (rsWireframe)
		rsWireframe->Release();
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

			camera.update(dt);

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

void renderFrame(double dt)
{
	static double elapTime = 0.0;
	elapTime += dt;

	SceneVariables sv = { camera.getViewProjection(), elapTime };
	DXDeviceInstance::get().getDevCon()->UpdateSubresource(constantBuffer.getBuffer(), 0, NULL, (void*)&sv, 0, 0);

	Color clearColor = Color(0.0f, 0.2f, 0.4f, 1.0f);
	DXDeviceInstance::get().getDevCon()->ClearRenderTargetView(DXDeviceInstance::get().getBackbuffer(), &clearColor[0]);
	DXDeviceInstance::get().getDevCon()->ClearDepthStencilView(DXDeviceInstance::get().getDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	// Draw 
	unsigned int strides[2] = { sizeof(VERTEX), sizeof(Instance) };
	unsigned int offsets[2] = { 0, 0 };
	ID3D11Buffer* bufferPointers[2] = { vertexBuffer.getBuffer(), instanceBuffer.getBuffer() };
	ID3D11Buffer* cBuffers = constantBuffer.getBuffer();

	//testShader.prepare();
	//DXDeviceInstance::get().getDevCon()->IASetVertexBuffers(0, 2, &bufferPointers[0], &strides[0], &offsets[0]);
	//DXDeviceInstance::get().getDevCon()->VSSetConstantBuffers(0, 1, &cBuffers);
	//DXDeviceInstance::get().getDevCon()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DXDeviceInstance::get().getDevCon()->Draw(3, 0);

	// Draw instanced
	instanceShader.prepare();
	DXDeviceInstance::get().getDevCon()->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	DXDeviceInstance::get().getDevCon()->IASetIndexBuffer(indexBuffer.getBuffer(), DXGI_FORMAT_R32_UINT, 0);
	DXDeviceInstance::get().getDevCon()->VSSetConstantBuffers(0, 1, &cBuffers);
	DXDeviceInstance::get().getDevCon()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXDeviceInstance::get().getDevCon()->RSSetState(rsNormal);
	DXDeviceInstance::get().getDevCon()->DrawIndexedInstanced(6, INSTANCE_COUNT, 0, 0, 0);

	testShader.prepare();
	DXDeviceInstance::get().getDevCon()->RSSetState(rsWireframe);
	DXDeviceInstance::get().getDevCon()->DrawIndexedInstanced(6, INSTANCE_COUNT, 0, 0, 0);

	DXDeviceInstance::get().getSwapchain()->Present(0, 0);
}

bool initPipeline()
{
	// Create input layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 2, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 3, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, sizeof(Matrix), D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	testShader.init("Instanced_V.hlsl", "FlashingColor_P.hlsl", ied);
	instanceShader.init("Instanced_V.hlsl", "SimpleColor_P.hlsl", ied);
	constantBuffer.init(NULL, sizeof(SceneVariables), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0);
	
	// Prepare instancing
	for (int i = 0; i < INSTANCE_COUNT; i++)
	{
		instances[i].position = Matrix::CreateTranslation(Vector3(0.5 * i, 0.5 * i, 0.5 * i));
		instances[i].color = Color(1.0f, 0.1f * i, 0.0f, 1.0f);
	}
	
	instanceBuffer.init(instances, sizeof(Instance) * INSTANCE_COUNT, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);

	// Render state
	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(D3D11_RASTERIZER_DESC));


	// Solid renderstate
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_NONE;
	HRESULT hr = DXDeviceInstance::get().getDev()->CreateRasterizerState(&rDesc, &rsNormal);
	if (FAILED(hr))
		return false;
	// Wireframe
	rDesc.FillMode = D3D11_FILL_WIREFRAME;
	rDesc.CullMode = D3D11_CULL_NONE;
	hr = DXDeviceInstance::get().getDev()->CreateRasterizerState(&rDesc, &rsWireframe);
	if (FAILED(hr))
		return false;


	return true;
}


bool initGraphics()
{
	// ############## VERTICES ###################
	VERTEX vertices[] =
	{
		{0.5f, 0.5f, 0.0f},
		{0.5f, -0.5, 0.0f},
		{-0.5f, -0.5f, 0.0f},
		{-0.5f, 0.5f, 0.0f},
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 3, 2
	};

	if (!vertexBuffer.init(vertices, sizeof(vertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0))
		return false;

	if (!indexBuffer.init(indices, sizeof(indices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0))
		return false;

	if (!constantBuffer.init(NULL, sizeof(SceneVariables), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0))
		return false;

	// Prepare instancing
	for (int i = 0; i < INSTANCE_COUNT; i++)
	{
		instances[i].position = Matrix::CreateTranslation(Vector3(0.5 * i, 0.5 * i, 0.5 * i));
		instances[i].color = Color(1.0f, 0.1f * i, 0.0f, 1.0f);
	}

	if (!instanceBuffer.init(instances, sizeof(Instance) * INSTANCE_COUNT, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0))
		return false;


	return true;
}