#pragma once


class DXSampler
{
public:
	DXSampler();
	~DXSampler();

	bool Init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
	const ComPtr<ID3D11SamplerState>& getSampler();

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};