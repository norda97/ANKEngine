#pragma once

class DXDeviceInstance 
{
private:
	DXDeviceInstance() = default;
	DXDeviceInstance(const DXDeviceInstance& other) = delete;
	DXDeviceInstance(DXDeviceInstance&& other) = delete;

public:
	~DXDeviceInstance();

	static DXDeviceInstance& Get() { return s_Instance; }

	static bool Init(HWND hWnd);
	static bool Release();

	static void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height);

	static const ComPtr<ID3D11Device>&				GetDev() { return s_Device;};
	static const ComPtr<ID3D11DeviceContext>&		GetDevCon() { return s_Devcon; };
	static const ComPtr<IDXGISwapChain>&			GetSwapchain() { return s_Swapchain; };
	static const ComPtr<ID3D11RenderTargetView>&	GetBackbuffer() { return s_Backbuffer; };
	static const ComPtr<ID3D11DepthStencilView>&	GetDepthStencilView() { return s_DepthStencilView; };
	
	static void HandleErrorMessage();

	static HWND GetHWND();
private:
	static D3D11_MESSAGE*	errorMsg;

	static HWND hWnd;
	static ComPtr<ID3D11Device>					s_Device;
	static ComPtr<ID3D11DeviceContext>			s_Devcon;
	static ComPtr<IDXGISwapChain>				s_Swapchain;
	static ComPtr<ID3D11InfoQueue>				s_InfoQueue;
		
	static ComPtr<ID3D11RenderTargetView>		s_Backbuffer;
	static ComPtr<ID3D11DepthStencilView>		s_DepthStencilView;
	static ComPtr<ID3D11Texture2D>				s_DepthStencilBuffer;

	static DXDeviceInstance						s_Instance;
};