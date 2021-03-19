#pragma once

class DXDeviceInstance 
{
private:
	DXDeviceInstance() = default;
	DXDeviceInstance(const DXDeviceInstance& other) = delete;
	DXDeviceInstance(DXDeviceInstance&& other) = delete;

public:
	~DXDeviceInstance();

	static DXDeviceInstance& get();

	void Init(HWND hWnd);

	void setViewport(unsigned x, unsigned y, unsigned width, unsigned height);

	const ComPtr<ID3D11Device>&				getDev();
	const ComPtr<ID3D11DeviceContext>&		getDevCon();
	const ComPtr<IDXGISwapChain>&			getSwapchain();
	const ComPtr<ID3D11RenderTargetView>&	getBackbuffer();
	const ComPtr<ID3D11DepthStencilView>&	getDepthStencilView();
	
	void handleErrorMessage();

	HWND getHWND();
private:
	D3D11_MESSAGE* errorMsg = nullptr;

	HWND hWnd = NULL;
	Microsoft::WRL::ComPtr<ID3D11Device>				m_Device				= nullptr;           
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_Devcon				= nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_Swapchain			= nullptr;
	Microsoft::WRL::ComPtr<ID3D11InfoQueue>				infoQueue			= nullptr;
		
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_Backbuffer			= nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_DepthStencilView	= nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_DepthStencilBuffer	= nullptr;

};