#pragma once

class DXDeviceInstance 
{
public:
	~DXDeviceInstance();

	static DXDeviceInstance& get() { return s_Instance; }

	static bool init(HWND hWnd);
	static bool release();

	static void setViewport(unsigned x, unsigned y, unsigned width, unsigned height);

	static const ComPtr<ID3D11Device>&				getDev() { return s_Device;};
	static const ComPtr<ID3D11DeviceContext>&		getDevCon() { return s_Devcon; };
	static const ComPtr<IDXGISwapChain>&			getSwapchain() { return s_Swapchain; };
	static const ComPtr<ID3D11RenderTargetView>&	getBackbuffer() { return s_Backbuffer; };
	static const ComPtr<ID3D11DepthStencilView>&	getDepthStencilView() { return s_DepthStencilView; };
	
	static void handleErrorMessage();

	static HWND getHWND();
private:
	DXDeviceInstance() {};
	DXDeviceInstance(const DXDeviceInstance& other) = delete;
	DXDeviceInstance(DXDeviceInstance&& other) = delete;

	static D3D11_MESSAGE* errorMsg;

	static HWND hWnd;
	static Microsoft::WRL::ComPtr<ID3D11Device>					s_Device;
	static Microsoft::WRL::ComPtr<ID3D11DeviceContext>			s_Devcon;
	static Microsoft::WRL::ComPtr<IDXGISwapChain>				s_Swapchain;
	static Microsoft::WRL::ComPtr<ID3D11InfoQueue>				s_InfoQueue;
		
	static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		s_Backbuffer;
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		s_DepthStencilView;
	static Microsoft::WRL::ComPtr<ID3D11Texture2D>				s_DepthStencilBuffer;

	static DXDeviceInstance s_Instance;
};