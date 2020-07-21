#include "pch.h"
#include "DXDeviceInstance.h"

DXDeviceInstance::~DXDeviceInstance()
{
	free(this->errorMsg);

	this->swapchain->SetFullscreenState(FALSE, NULL);

//#ifdef ANK_DEBUG
//	{
//		ComPtr<ID3D11Debug> debug;
//		HRESULT hr = DXDeviceInstance::get().getDev()->QueryInterface(IID_PPV_ARGS(&debug));
//		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//	}
//#endif
}

DXDeviceInstance& DXDeviceInstance::get()
{
	static DXDeviceInstance i;

	return i;
}

void DXDeviceInstance::init(HWND hWnd)
{
	this->hWnd = hWnd;

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
		this->swapchain.GetAddressOf(),
		this->device.GetAddressOf(),
		NULL,
		this->devcon.GetAddressOf());

	ComPtr<ID3D11Texture2D> pBackBuffer;
	this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());

	// Create InfoQueue interface
#if ANK_DEBUG
	ANK_ASSERT(SUCCEEDED(this->device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)this->infoQueue.GetAddressOf())), "Failed to query infoQueue from device!\n");
#endif

	HRESULT hr = this->device->CreateRenderTargetView(pBackBuffer.Get(), NULL, this->backbuffer.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create render target view");
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

	hr = this->device->CreateTexture2D(&texDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil buffer");
	}

	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	//depthStencilViewDesc.Format = texDesc.Format;
	//depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	//depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil view");
	}

	setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
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

	this->devcon->RSSetViewports(1, &viewport);
}

void DXDeviceInstance::handleErrorMessage()
{
	if (SUCCEEDED(this->infoQueue->PushEmptyStorageFilter())) 
	{
		UINT64 msgCount = this->infoQueue->GetNumStoredMessages();

		for (UINT64 i = 0; i < msgCount; i++)
		{
			static SIZE_T msgSize = 0;
			SIZE_T newMsgSize = 0;
			this->infoQueue->GetMessage(i, NULL, &newMsgSize);

			if (newMsgSize > msgSize) 
			{
				free(this->errorMsg);
				msgSize = newMsgSize;
				this->errorMsg = (D3D11_MESSAGE*)malloc(msgSize);
			}

			if (msgSize > 0)
			{
				HRESULT hr = this->infoQueue->GetMessage(i, this->errorMsg, &msgSize);
				if (FAILED(hr))
					ANK_ERROR("Failed to retrieve message from ID3D11InfoQueue\n");

				ANK_INFO(": %.*s\n", this->errorMsg->DescriptionByteLength, this->errorMsg->pDescription);
			}
		}
		this->infoQueue->ClearStoredMessages();
	}
}

const ComPtr<ID3D11Device>& DXDeviceInstance::getDev()
{
	return this->device;
}

const ComPtr<ID3D11DeviceContext>& DXDeviceInstance::getDevCon()
{
	return this->devcon;
}

const ComPtr<IDXGISwapChain>& DXDeviceInstance::getSwapchain()
{
	return this->swapchain;
}

const ComPtr<ID3D11RenderTargetView>& DXDeviceInstance::getBackbuffer()
{
	return this->backbuffer;
}

const ComPtr<ID3D11DepthStencilView>& DXDeviceInstance::getDepthStencilView()
{
	return this->depthStencilView;
}


HWND DXDeviceInstance::getHWND()
{
	return this->hWnd;
}

