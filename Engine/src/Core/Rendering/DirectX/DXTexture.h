#pragma once


class DXTexture
{
public:
	DXTexture();
	~DXTexture();

	bool isInitilized() const;
	bool init(D3D11_SUBRESOURCE_DATA* pData, const D3D11_TEXTURE2D_DESC& tdesc);
	bool loadTexture(const std::string& path);

	const ComPtr<ID3D11ShaderResourceView>& getShaderResource() const;
	const ComPtr<ID3D11Texture2D>& getTexture() const;

private:
	bool initialized;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ResourceView;
};

