#include "pch.h"
#include "DXDeviceInstance.h"


// Static variable initilization
DXDeviceInstance DXDeviceInstance::s_Instance;

Microsoft::WRL::ComPtr<ID3D11Device>				DXDeviceInstance::s_Device;
Microsoft::WRL::ComPtr<ID3D11DeviceContext>			DXDeviceInstance::s_Devcon;
Microsoft::WRL::ComPtr<IDXGISwapChain>				DXDeviceInstance::s_Swapchain;
Microsoft::WRL::ComPtr<ID3D11InfoQueue>				DXDeviceInstance::s_InfoQueue;

Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		DXDeviceInstance::s_Backbuffer;
Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		DXDeviceInstance::s_DepthStencilView;
Microsoft::WRL::ComPtr<ID3D11Texture2D>				DXDeviceInstance::s_DepthStencilBuffer;

D3D11_MESSAGE* DXDeviceInstance::errorMsg;
HWND DXDeviceInstance::hWnd;

DXDeviceInstance::~DXDeviceInstance()
{
}

bool DXDeviceInstance::init(HWND hWnd)
{
	hWnd = hWnd;

	UINT deviceFlags = 0;

	// Turn on D3D11 debugging
#ifdef ANK_DEBUG
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
	scd.SampleDesc.Count = 1;           
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;                             
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		deviceFlags,
		featureLevels,
		2,
		D3D11_SDK_VERSION,
		&scd,
		s_Swapchain.GetAddressOf(),
		s_Device.GetAddressOf(),
		NULL,
		s_Devcon.GetAddressOf());

	ComPtr<ID3D11Texture2D> pBackBuffer;
	s_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());

	// Create InfoQueue interface
#if ANK_DEBUG
	ANK_ASSERT(SUCCEEDED(s_Device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)s_InfoQueue.GetAddressOf())), "Failed to query infoQueue from device!\n");
#endif

	HRESULT hr = s_Device->CreateRenderTargetView(pBackBuffer.Get(), NULL, s_Backbuffer.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create render target view");
		return false;
	}

	// Depth testing setup
	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = SCREEN_WIDTH;
	texDesc.Height = SCREEN_HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = s_Device->CreateTexture2D(&texDesc, NULL, s_DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil buffer");
		return false;
	}

	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	//depthStencilViewDesc.Format = texDesc.Format;
	//depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	//depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = s_Device->CreateDepthStencilView(s_DepthStencilBuffer.Get(), NULL, s_DepthStencilView.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil view");
		return false;
	}

	setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;
}

bool DXDeviceInstance::release()
{
	free(errorMsg);
	if(!SUCCEEDED(s_Swapchain->SetFullscreenState(FALSE, NULL)));
		return false;

	return true;
}

void DXDeviceInstance::setViewport(unsigned x, unsigned y, unsigned width, unsigned height)
{
	D3D11_VIEWPORT viewport = {};

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;

	s_Devcon->RSSetViewports(1, &viewport);
}

void DXDeviceInstance::handleErrorMessage()
{
	if (SUCCEEDED(s_InfoQueue->PushEmptyStorageFilter())) 
	{
		UINT64 msgCount = s_InfoQueue->GetNumStoredMessages();

		for (UINT64 i = 0; i < msgCount; i++)
		{
			static SIZE_T msgSize = 0;
			SIZE_T newMsgSize = 0;
			s_InfoQueue->GetMessage(i, NULL, &newMsgSize);

			if (newMsgSize > msgSize) 
			{
				free(errorMsg);
				msgSize = newMsgSize;
				errorMsg = (D3D11_MESSAGE*)malloc(msgSize);
			}

			if (msgSize > 0)
			{
				HRESULT hr = s_InfoQueue->GetMessage(i, errorMsg, &msgSize);
				if (FAILED(hr))
					ANK_ERROR("Failed to retrieve message from ID3D11InfoQueue\n");

				ANK_INFO(": %.*s\n", errorMsg->DescriptionByteLength, errorMsg->pDescription);
			}
		}
		s_InfoQueue->ClearStoredMessages();
	}
}

HWND DXDeviceInstance::getHWND()
{
	return hWnd;
}


