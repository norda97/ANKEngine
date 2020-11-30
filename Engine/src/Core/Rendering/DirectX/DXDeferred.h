#pragma once

#include "DXShader.h"
#include "Core/Model/Model.h"
#include "Core/Model/Mesh.h"
#include "Core/Rendering/DirectX/DXBuffer.h"

class DXDeferred
{
private:
	static const unsigned GBUFFER_COUNT = 4;
public:
	DXDeferred();
	~DXDeferred();

	void clearRenderTargets();
	void bindRenderTargets(ID3D11DepthStencilView* depthStencil);
	const std::array<ID3D11RenderTargetView*, GBUFFER_COUNT>& getRenderTargets();
	void bindShaders();
	const std::array<ID3D11ShaderResourceView*, GBUFFER_COUNT>& getResourceViews() const;

	void renderComplete(ID3D11RenderTargetView* const* renderTarget);

private:
	bool initShaders();
	void initFullscreenTri();

	DXShader fullscreenShader;
	DXBuffer fullscreenTri;

	DXShader shader;

	std::vector<ComPtr<ID3D11Texture2D>>					gBuffers;
	std::vector<ComPtr<ID3D11RenderTargetView>>				renderTargets;
	std::vector<ComPtr<ID3D11ShaderResourceView>>			resourceView;

	std::array<ID3D11RenderTargetView*, GBUFFER_COUNT>		ptrRenderTargets;
	std::array<ID3D11ShaderResourceView*, GBUFFER_COUNT>	ptrResourceViews;
};