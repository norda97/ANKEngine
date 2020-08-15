#include "pch.h"
#include "DXCubemap.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#include "Core/Utils/Cameras/Camera.h"

DXCubemap::DXCubemap() :
	initialized(false),
	width(0),
	height(0),
	mipLevels(1)
{
}

DXCubemap::~DXCubemap()
{
}

bool DXCubemap::init(unsigned width, unsigned height, unsigned mipLevels)
{
	if (!initialized) {
		auto& devcon = DXDeviceInstance::get().getDevCon();
		auto& dev = DXDeviceInstance::get().getDev();

		this->mipLevels = mipLevels;
		this->width = width;
		this->height = height;

		D3D11_TEXTURE2D_DESC tdesc = { 0 };
		tdesc.Width = width;
		tdesc.Height = height;
		tdesc.MipLevels = mipLevels;
		tdesc.ArraySize = 6;
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tdesc.CPUAccessFlags = 0;
		tdesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = NULL;
		srd.SysMemPitch = width * 4;
		srd.SysMemSlicePitch = 0;

		HRESULT hr = DXDeviceInstance::get().getDev()->CreateTexture2D(&tdesc, NULL, this->cubemap.ReleaseAndGetAddressOf());


		if (FAILED(hr)) {
			ANK_ERROR("Failed to create texture2D\n");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		ZeroMemory(&srDesc, sizeof(srDesc));
		srDesc.Format = tdesc.Format;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = tdesc.MipLevels;

		hr = DXDeviceInstance::get().getDev()->CreateShaderResourceView(this->cubemap.Get(), &srDesc, this->resourceView.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			ANK_ERROR("Failed to create shader resource view\n");
			return false;
		}

		this->renderTargets.resize(6);
		this->resourceViews.resize(6);
		for (unsigned i = 0; i < 6; i++)
		{
			this->renderTargets[i].resize(this->mipLevels);
			for (unsigned j = 0; j < this->mipLevels; j++)
			{
				D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
				rtvDesc.Format = tdesc.Format;
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.MipSlice = j;
				rtvDesc.Texture2DArray.ArraySize = 1;
				rtvDesc.Texture2DArray.FirstArraySlice = i;

				//.MipSlice = D3D11CalcSubresource(0, 6, tdesc.MipLevels);
				ANK_ASSERT(SUCCEEDED(dev->CreateRenderTargetView(this->cubemap.Get(), &rtvDesc, this->renderTargets[i][j].ReleaseAndGetAddressOf())),
					"Failed to create render target views  for cubemap");
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
			ZeroMemory(&srDesc, sizeof(srDesc));
			srDesc.Format = tdesc.Format;
			srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srDesc.Texture2DArray.MipLevels = this->mipLevels;
			srDesc.Texture2DArray.MostDetailedMip = 0;
			srDesc.Texture2DArray.ArraySize = 1;
			srDesc.Texture2DArray.FirstArraySlice = i;

			ANK_ASSERT(SUCCEEDED(DXDeviceInstance::get().getDev()->CreateShaderResourceView(this->cubemap.Get(), &srDesc, this->resourceViews[i].ReleaseAndGetAddressOf())),
				"Failed to create resource views for cubemap");

		}
		initialized = true;
	}
	else
	{
		ANK_WARNING("Cubemap already initilized\n");
	}

	return true;
}

unsigned DXCubemap::getMipLevels() const
{
	return this->mipLevels;
}

const std::vector<std::vector<ComPtr<ID3D11RenderTargetView>>>& DXCubemap::getRenderTargets()
{
	return this->renderTargets;
}

const ComPtr<ID3D11ShaderResourceView>& DXCubemap::getResourceView()
{
	return this->resourceView;
}

const std::vector<ComPtr<ID3D11ShaderResourceView>>& DXCubemap::getResourceViews()
{
	return this->resourceViews;
}

