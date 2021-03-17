#include "pch.h"
#include "DXTexture.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

DXTexture::DXTexture() : initialized(false)
{
}

DXTexture::~DXTexture()
{
	//if (this->texture)
	//	this->texture->Release();

	//if (this->m_ResourceView)
	//	this->m_ResourceView->Release();
}

bool DXTexture::isInitilized() const
{
	return this->initialized;
}

bool DXTexture::Init(D3D11_SUBRESOURCE_DATA* pData, const D3D11_TEXTURE2D_DESC& tdesc)
{
	HRESULT hr = DXDeviceInstance::GetDev()->CreateTexture2D(&tdesc, pData, this->texture.ReleaseAndGetAddressOf());

	if (FAILED(hr)) {
		LOG_ERROR("Failed to create texture2D");
		return false;
	}
	D3D11_SRV_DIMENSION viewDim = D3D11_SRV_DIMENSION_TEXTURE2D;
	if (tdesc.Format == D3D11_RESOURCE_MISC_TEXTURECUBE) {
		viewDim = D3D11_SRV_DIMENSION_TEXTURE2D;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = tdesc.Format;
	srDesc.ViewDimension = viewDim;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = DXDeviceInstance::GetDev()->CreateShaderResourceView(this->texture.Get(), &srDesc, m_ResourceView.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		LOG_ERROR("Failed to create shader resource view");
		return false;
	}

	initialized = true;
	return true;
}

bool DXTexture::loadTexture(const std::string& path)
{
	int width, height, channels;
	unsigned char* image = stbi_load(path.c_str(),
		&width,
		&height,
		&channels,
		STBI_rgb_alpha);

	if(!image)
	{ 
		LOG_ERROR("Failed to load texture from file: %s", path.c_str());
		return false;
	}

	//DXDeviceInstance::GetDev()->CheckMultisampleQualityLevels()

	
	D3D11_TEXTURE2D_DESC tdesc = { 0 };
	tdesc.Width = width;
	tdesc.Height = height;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.Usage = D3D11_USAGE_IMMUTABLE; // Immutable = only read by GPU and constant during existence
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tdesc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GENERATE_MIPS for mip map
	tdesc.CPUAccessFlags = 0; 

	D3D11_SUBRESOURCE_DATA srd = { 0 };
	srd.pSysMem = (void*)image;
	srd.SysMemPitch = width * 4;
	srd.SysMemSlicePitch = 0;

	HRESULT hr = DXDeviceInstance::GetDev()->CreateTexture2D(&tdesc, &srd, this->texture.ReleaseAndGetAddressOf());

	stbi_image_free(image);

	if (FAILED(hr)) {
		LOG_ERROR("Failed to create texture2D");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = tdesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = DXDeviceInstance::GetDev()->CreateShaderResourceView(this->texture.Get(), &srDesc, m_ResourceView.ReleaseAndGetAddressOf());
	if(FAILED(hr)) {
		LOG_ERROR("Failed to create shader resource view");
		return false;
	}

	initialized = true;

	return initialized;
}

const ComPtr<ID3D11ShaderResourceView>& DXTexture::getShaderResource() const
{
	return this->m_ResourceView;
}

const ComPtr<ID3D11Texture2D>& DXTexture::getTexture() const
{
	return this->texture;
}

