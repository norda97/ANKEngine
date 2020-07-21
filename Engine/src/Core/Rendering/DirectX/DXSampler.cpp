#include "pch.h"
#include "DXSampler.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

DXSampler::DXSampler()
{
}

DXSampler::~DXSampler()
{
	//if (this->samplerState)
	//	this->samplerState->Release();
}

bool DXSampler::init()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MaxAnisotropy = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = DXDeviceInstance::get().getDev()->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());

	if (FAILED(hr)) {
		ANK_ERROR("Failed to create SamplerState\n");
		return false;
	}

	return true;
}

const ComPtr<ID3D11SamplerState>& DXSampler::getSampler()
{
	return this->samplerState;
}