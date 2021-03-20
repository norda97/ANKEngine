#pragma once


class DXTexture
{
public:
	DXTexture();
	~DXTexture();

	bool isInitilized() const;
	bool Init(D3D11_SUBRESOURCE_DATA* pData, const D3D11_TEXTURE2D_DESC& tdesc);
	bool loadTexture(const std::string& path);

	const ComPtr<ID3D11ShaderResourceView>& getShaderResource() const;
	ComPtr<ID3D11ShaderResourceView>& getShaderResource();
	const ComPtr<ID3D11Texture2D>& Get() const;
	ComPtr<ID3D11Texture2D>& Get();

private:
	bool initialized;

	ComPtr<ID3D11Texture2D> m_Texture2D;
	ComPtr<ID3D11ShaderResourceView> m_ResourceView;
};

