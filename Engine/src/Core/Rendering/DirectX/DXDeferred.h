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

	bool ResizeGBuffers(float width, float height);

	void ClearRenderTargets();
	void BindRenderTargets(ID3D11DepthStencilView* depthStencil);
	const std::array<ID3D11RenderTargetView*, GBUFFER_COUNT>& GetRenderTargets();
	void bindShaders();
	const std::array<ID3D11ShaderResourceView*, GBUFFER_COUNT>& GetResourceViews() const;

	void RenderComplete(ID3D11RenderTargetView* const* renderTarget);

private:
	bool InitShaders();
	void InitFullscreenTri();

	DXShader m_FullscreenShader;
	DXBuffer m_FullscreenTri;

	DXShader shader;

	std::vector<ComPtr<ID3D11Texture2D>>					m_GeomBuffers;
	std::vector<ComPtr<ID3D11RenderTargetView>>				m_RenderTargets;
	std::vector<ComPtr<ID3D11ShaderResourceView>>			m_ResourceView;

	std::array<ID3D11RenderTargetView*, GBUFFER_COUNT>		m_pRenderTargets;
	std::array<ID3D11ShaderResourceView*, GBUFFER_COUNT>	m_pResourceViews;
};