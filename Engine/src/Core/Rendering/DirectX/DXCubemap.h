#pragma once

#include "Core/Rendering/DirectX/DXTexture.h"

class DXCubemap
{
public:
	unsigned m_Width, m_Height;
public:
	DXCubemap();
	~DXCubemap();

	//bool Init(unsigned width, unsigned height);
	bool Init(unsigned width, unsigned height, unsigned mipLevels);

	unsigned getMipLevels() const;

	const std::vector<std::vector<ComPtr<ID3D11RenderTargetView>>>& GetRenderTargets();
	const ComPtr<ID3D11ShaderResourceView>& getResourceView();
	const std::vector<ComPtr<ID3D11ShaderResourceView>>& GetResourceViews();

private:
	bool initialized;

	unsigned m_MipLevels;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Cubemap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ResourceView;
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_ResourceViews;
	std::vector<std::vector<ComPtr<ID3D11RenderTargetView>>> m_RenderTargets;
};