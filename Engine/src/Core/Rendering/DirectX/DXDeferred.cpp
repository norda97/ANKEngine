#include "pch.h"
#include "DXDeferred.h"

#include "DXDeviceInstance.h"
#include "DXMaterial.h"

#include "Core/Model/MeshInstance.h"
#include "Core/Model/Mesh.h"

DXDeferred::DXDeferred()
{
	auto& devcon = DXDeviceInstance::get().getDevCon();
	auto& dev = DXDeviceInstance::get().getDev();

	this->gBuffers.resize(this->GBUFFER_COUNT);
	this->renderTargets.resize(this->GBUFFER_COUNT);
	this->resourceView.resize(this->GBUFFER_COUNT);

	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = SCREEN_WIDTH;
	texDesc.Height = SCREEN_HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	DXGI_FORMAT gbufferFormats[] = {
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM
	};

	for (unsigned i = 0; i < this->GBUFFER_COUNT; i++)
	{
		texDesc.Format = rtvDesc.Format = srvDesc.Format = gbufferFormats[i];

		// Only world positon texture needs 16 bytes texture, normals and 
		// Create textures
		HRESULT hr = dev->CreateTexture2D(&texDesc, NULL, this->gBuffers[i].GetAddressOf());
		if (FAILED(hr)) {
			ANK_ERROR("Failed to create depth stencil buffer");
		}

		// Create render target
		hr = dev->CreateRenderTargetView(this->gBuffers[i].Get(), &rtvDesc, this->renderTargets[i].GetAddressOf());
		if (FAILED(hr)) {
			ANK_ERROR("Failed to create render target view");
		}
		this->ptrRenderTargets[i] = this->renderTargets[i].Get();

		// Create resource view
		hr = dev->CreateShaderResourceView(this->gBuffers[i].Get(), &srvDesc, this->resourceView[i].GetAddressOf());
		if (FAILED(hr)) {
			ANK_ERROR("Failed to create resource view");
		}
		this->ptrResourceViews[i] = this->resourceView[i].Get();
	}

	initShaders();
	initFullscreenTri();
}

DXDeferred::~DXDeferred()
{
}

void DXDeferred::clearRenderTargets()
{
	auto& devcon = DXDeviceInstance::get().getDevCon();
	Color clearColor = Color(0.01f, 0.01f, 0.01f, 0.0f);

	devcon->ClearRenderTargetView(this->ptrRenderTargets[0], &clearColor[0]);
	devcon->ClearRenderTargetView(this->ptrRenderTargets[1], &clearColor[0]);
	devcon->ClearRenderTargetView(this->ptrRenderTargets[2], &clearColor[0]);
	devcon->ClearRenderTargetView(this->ptrRenderTargets[3], &clearColor[0]);
}

void DXDeferred::bindRenderTargets(ID3D11DepthStencilView* depthStencil)
{
	DXDeviceInstance::get().getDevCon()->OMSetRenderTargets(this->GBUFFER_COUNT, this->ptrRenderTargets.data(), depthStencil);
}

const std::array<ID3D11RenderTargetView*, 4>& DXDeferred::getRenderTargets()
{
	return this->ptrRenderTargets;
}

void DXDeferred::bindShaders()
{
	this->shader.prepare();
}

const std::array<ID3D11ShaderResourceView*, 4>& DXDeferred::getResourceViews() const
{
	return this->ptrResourceViews;
}

void DXDeferred::renderMeshInstanced(const Mesh& mesh, unsigned instanceCount, unsigned instanceOffset)
{
	//auto& devcon = DXDeviceInstance::get().getDevCon();

	//unsigned int strides[1] = { sizeof(VertexData) };
	//unsigned int offsets[1] = { 0 };
	//ID3D11Buffer* bufferPointers[1] = { static_cast<const DXBuffer*>(mesh.getVertexBuffer())->getBuffer().Get() };

	//devcon->IASetVertexBuffers(0, 1, bufferPointers, strides, offsets);
	//devcon->IASetIndexBuffer(static_cast<const DXBuffer*>(mesh.getIndexBuffer())->getBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	//devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	////DXDeviceInstance::get().getDevCon()->RSSetState(rsWireframe);
	//devcon->DrawIndexedInstanced(mesh.getIndexCount(), instanceCount, 0, 0, instanceOffset);
}

void DXDeferred::renderComplete(ID3D11RenderTargetView* const* renderTarget)
{
	auto& devcon = DXDeviceInstance::get().getDevCon();

	this->fullscreenShader.prepare();

	// Render fullscreen tri to backbuffer
	devcon->OMSetRenderTargets(1, renderTarget, NULL);
	devcon->PSSetShaderResources(0, this->GBUFFER_COUNT, this->ptrResourceViews.data());

	unsigned int strides[1] = { sizeof(float) * 5 };
	unsigned int offsets[1] = { 0 };

	devcon->IASetVertexBuffers(0, 1, fullscreenTri.getBuffer().GetAddressOf(), strides, offsets);

	devcon->Draw(3, 0);

	// Unbind shader resources 
	ID3D11ShaderResourceView* nullSRV[4] = { nullptr, nullptr, nullptr, nullptr };
	devcon->PSSetShaderResources(0, 4, nullSRV);
}

bool DXDeferred::initShaders()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(Vector3) * 1, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(Vector3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector3) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},

		// Instanced
		{"INSTANCE_WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 1, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 2, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 3, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	if (!this->shader.init("Deferred/Prerendering_V.hlsl", "Deferred/Prerendering_P.hlsl", ied)) {
		ANK_WARNING("Failed to load deferred prerendering shader!");
		return false;
	}

	ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector3) * 1, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (!this->fullscreenShader.init("UtilShaders/FullscreenQuad_V.hlsl", "Deferred/DeferredPBR_IBL_P.hlsl", ied))
		return false;

	return true;
}

void DXDeferred::initFullscreenTri()
{
	float vertices[]
	{
		// Pos				// Uv
		-1.f, -1.f, 0.f,	0.f, 0.f,
		-1.f, 3.f, 0.f,		0.f, -2.f,
		3.f, -1.f, 0.f,		2.f, 0.f
	};

	this->fullscreenTri.init(&vertices, sizeof(float) * 18, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);
}
