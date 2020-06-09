#include "pch.h"
#include "DXDeviceInstance.h"

DXDeviceInstance::~DXDeviceInstance()
{
	this->swapchain->SetFullscreenState(FALSE, NULL);

	if (this->device)
		device->Release();
	if (this->devcon)
		devcon->Release();
	if (this->swapchain)
		swapchain->Release();
	if (this->backbuffer)
		backbuffer->Release();

}

DXDeviceInstance& DXDeviceInstance::get()
{
	static DXDeviceInstance i;

	return i;
}

void DXDeviceInstance::init(HWND hWnd)
{
	this->hWnd;

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = { 0 };

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH;                    // set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;                  // set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.SampleDesc.Quality = 0;                             // multisample quality level
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&this->swapchain,
		&this->device,
		NULL,
		&this->devcon);

	// get the address of the back buffer
	ID3D11Texture2D* pBackBuffer;
	this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	this->device->CreateRenderTargetView(pBackBuffer, NULL, &this->backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	this->devcon->OMSetRenderTargets(1, &this->backbuffer, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport = {};

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	this->devcon->RSSetViewports(1, &viewport);
}

ID3D11Device* DXDeviceInstance::getDev()
{
	return this->device;
}

ID3D11DeviceContext* DXDeviceInstance::getDevCon()
{
	return this->devcon;
}

IDXGISwapChain* DXDeviceInstance::getSwapchain()
{
	return this->swapchain;
}

ID3D11RenderTargetView* DXDeviceInstance::getBackbuffer()
{
	return this->backbuffer;
}

HWND DXDeviceInstance::getHWND()
{
	return this->hWnd;
}
