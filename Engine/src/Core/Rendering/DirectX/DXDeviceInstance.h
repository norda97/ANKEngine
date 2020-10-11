#pragma once

class DXDeviceInstance 
{
public:
	~DXDeviceInstance();

	static DXDeviceInstance& get();

	void init(HWND hWnd);

	void setViewport(unsigned x, unsigned y, unsigned width, unsigned height);

	const ComPtr<ID3D11Device>&				getDev();
	const ComPtr<ID3D11DeviceContext>&		getDevCon();
	const ComPtr<IDXGISwapChain>&			getSwapchain();
	const ComPtr<ID3D11RenderTargetView>&	getBackbuffer();
	const ComPtr<ID3D11DepthStencilView>&	getDepthStencilView();
	
	void handleErrorMessage();

	HWND getHWND();
private:
	DXDeviceInstance() {};
	DXDeviceInstance(const DXDeviceInstance& other) = delete;
	DXDeviceInstance(DXDeviceInstance&& other) = delete;

	D3D11_MESSAGE* errorMsg = nullptr;

	HWND hWnd = NULL;
	Microsoft::WRL::ComPtr<ID3D11Device>				device				= nullptr;           
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			devcon				= nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				swapchain			= nullptr;
	Microsoft::WRL::ComPtr<ID3D11InfoQueue>				infoQueue			= nullptr;
		
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		backbuffer			= nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depthStencilView	= nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				depthStencilBuffer	= nullptr;

};