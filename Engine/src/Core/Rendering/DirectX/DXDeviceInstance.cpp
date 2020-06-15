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
	if (this->depthStencilBuffer)
		depthStencilBuffer->Release();
	if (this->depthStencilView)
		depthStencilView->Release();
	if (this->depthStencilState)
		this->depthStencilState->Release();

}

DXDeviceInstance& DXDeviceInstance::get()
{
	static DXDeviceInstance i;

	return i;
}

void DXDeviceInstance::init(HWND hWnd)
{
	this->hWnd;

	UINT deviceFlags = 0;

	// Turn on D3D11 debugging
#ifdef MINI_DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Determines the order of feature levels to attempt to create
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

	// Swapchain description
	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferCount = 1; 
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;           
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;                             
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		deviceFlags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&this->swapchain,
		&this->device,
		NULL,
		&this->devcon);

	ID3D11Texture2D* pBackBuffer;
	this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	this->device->CreateRenderTargetView(pBackBuffer, NULL, &this->backbuffer);
	pBackBuffer->Release();

	// Depth testing setup
	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = SCREEN_WIDTH;
	texDesc.Height = SCREEN_HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count = 4;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = this->device->CreateTexture2D(&texDesc, NULL, &this->depthStencilBuffer);
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil buffer");
	}

	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	//depthStencilViewDesc.Format = texDesc.Format;
	//depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	//depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer, NULL, &this->depthStencilView);
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil view");
	}

	this->devcon->OMSetRenderTargets(1, &this->backbuffer, this->depthStencilView);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = false;

	hr = this->device->CreateDepthStencilState(&depthStencilDesc, &this->depthStencilState);
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil state");
	}
	this->devcon->OMSetDepthStencilState(this->depthStencilState, 0);

	D3D11_VIEWPORT viewport = {};

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
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

ID3D11DepthStencilView* DXDeviceInstance::getDepthStencilView()
{
	return this->depthStencilView;
}


HWND DXDeviceInstance::getHWND()
{
	return this->hWnd;
}
