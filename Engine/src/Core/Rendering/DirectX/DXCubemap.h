#pragma once

#include "Core/Rendering/DirectX/DXTexture.h"

class DXCubemap
{
public:
	unsigned width, height;
public:
	DXCubemap();
	~DXCubemap();

	//bool Init(unsigned width, unsigned height);
	bool Init(unsigned width, unsigned height, unsigned mipLevels);

	unsigned getMipLevels() const;

	const std::vector<std::vector<ComPtr<ID3D11RenderTargetView>>>& getRenderTargets();
	const ComPtr<ID3D11ShaderResourceView>& getResourceView();
	const std::vector<ComPtr<ID3D11ShaderResourceView>>& getResourceViews();

private:
	bool initialized;

	unsigned mipLevels;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubemap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;
	std::vector<ComPtr<ID3D11ShaderResourceView>> resourceViews;
	std::vector<std::vector<ComPtr<ID3D11RenderTargetView>>> renderTargets;
};