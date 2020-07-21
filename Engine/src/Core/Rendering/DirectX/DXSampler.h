#pragma once


class DXSampler
{
public:
	DXSampler();
	~DXSampler();

	bool init();
	const ComPtr<ID3D11SamplerState>& getSampler();

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};