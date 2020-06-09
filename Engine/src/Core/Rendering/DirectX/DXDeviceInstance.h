#pragma once

class DXDeviceInstance 
{
public:
	~DXDeviceInstance();

	static DXDeviceInstance& get();

	void init(HWND hWnd);

	ID3D11Device* getDev();
	ID3D11DeviceContext* getDevCon();
	IDXGISwapChain* getSwapchain();
	ID3D11RenderTargetView* getBackbuffer();
	
	HWND getHWND();

private:
	DXDeviceInstance() {};
	DXDeviceInstance(DXDeviceInstance& other) = delete;

	HWND hWnd;
	ID3D11Device*			device		= nullptr;           
	ID3D11DeviceContext*	devcon		= nullptr;
	IDXGISwapChain*			swapchain	= nullptr;
	ID3D11RenderTargetView* backbuffer	= nullptr;
};