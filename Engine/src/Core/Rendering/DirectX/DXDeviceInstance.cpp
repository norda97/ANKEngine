#include "pch.h"
#include "DXDeviceInstance.h"

#include "Core/Utils/ANKWindowHandler.h"

DXDeviceInstance::~DXDeviceInstance()
{
	free(errorMsg);

	if (m_Swapchain)
		m_Swapchain->SetFullscreenState(FALSE, NULL);

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
		m_Swapchain.GetAddressOf(),
		m_Device.GetAddressOf(),
		NULL,
		m_Devcon.GetAddressOf());

	// Create InfoQueue interface
#if ANK_DEBUG
	ANK_ASSERT(SUCCEEDED(m_Device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)infoQueue.GetAddressOf())), "Failed to query infoQueue from device!");
#endif

	ComPtr<ID3D11Texture2D> pBackBuffer;
	m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());

	HRESULT hr = m_Device->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_Backbuffer.GetAddressOf());
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

	hr = m_Device->CreateTexture2D(&texDesc, NULL, m_DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil buffer");
	}

	hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), NULL, m_DepthStencilView.GetAddressOf());
	if (FAILED(hr)) {
		ANK_ERROR("Failed to create depth stencil view");
	}

	setViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Register resize callback
	ANKWindowHandler::RegisterResizeCallback(
	[&](uint32_t width, uint32_t height) 
		{
			/*
				Resize backbuffers
			*/
			
			// Release old backbuffer ref
			m_Backbuffer->Release();

			// Resize backbuffers
			if (m_Swapchain != nullptr)
			{
				HRESULT hr = m_Swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
				if (FAILED(hr))
				{
					ANK_ERROR("Failed to resize swapchain buffers!");
				}
			}

			// Retrieve backbuffers
			ComPtr<ID3D11Texture2D> pBackBuffer;
			m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());

			HRESULT hr = m_Device->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_Backbuffer.GetAddressOf());
			if (FAILED(hr)) {
				ANK_ERROR("Failed to create render target view");
			}

			// Depth testing setup
			D3D11_TEXTURE2D_DESC texDesc = { 0 };
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			hr = m_Device->CreateTexture2D(&texDesc, NULL, m_DepthStencilBuffer.ReleaseAndGetAddressOf());
			if (FAILED(hr)) {
				ANK_ERROR("Failed to create depth stencil buffer");
			}

			hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), NULL, m_DepthStencilView.ReleaseAndGetAddressOf());
			if (FAILED(hr)) {
				ANK_ERROR("Failed to create depth stencil view");
			}
		});
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

	m_Devcon->RSSetViewports(1, &viewport);
}

void DXDeviceInstance::handleErrorMessage()
{
	if (SUCCEEDED(infoQueue->PushEmptyStorageFilter())) 
	{
		UINT64 msgCount = infoQueue->GetNumStoredMessages();

		for (UINT64 i = 0; i < msgCount; i++)
		{
			static SIZE_T msgSize = 0;
			SIZE_T newMsgSize = 0;
			infoQueue->GetMessage(i, NULL, &newMsgSize);

			if (newMsgSize > msgSize) 
			{
				free(errorMsg);
				msgSize = newMsgSize;
				errorMsg = (D3D11_MESSAGE*)malloc(msgSize);
			}

			if (msgSize > 0)
			{
				HRESULT hr = infoQueue->GetMessage(i, errorMsg, &msgSize);
				if (FAILED(hr))
					ANK_ERROR("Failed to retrieve message from ID3D11InfoQueue");

				ANK_INFO(": %.*s", errorMsg->DescriptionByteLength, errorMsg->pDescription);
			}
		}
		infoQueue->ClearStoredMessages();
	}
}

const ComPtr<ID3D11Device>& DXDeviceInstance::getDev()
{
	return m_Device;
}

const ComPtr<ID3D11DeviceContext>& DXDeviceInstance::getDevCon()
{
	return m_Devcon;
}

const ComPtr<IDXGISwapChain>& DXDeviceInstance::getSwapchain()
{
	return m_Swapchain;
}

const ComPtr<ID3D11RenderTargetView>& DXDeviceInstance::getBackbuffer()
{
	return m_Backbuffer;
}

const ComPtr<ID3D11DepthStencilView>& DXDeviceInstance::getDepthStencilView()
{
	return m_DepthStencilView;
}


HWND DXDeviceInstance::getHWND()
{
	return hWnd;
}


