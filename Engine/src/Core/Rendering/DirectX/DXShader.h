#pragma once

#define LOAD_PRECOMPILED_SHADERS false

class DXShader
{
public:
	DXShader();
	~DXShader();

	bool init(const std::string& vFilename, const std::string& pFilename, std::vector<D3D11_INPUT_ELEMENT_DESC>& ied);
	void prepare();
	
private:
	bool compileShader(const std::string& file, const std::string& entry, const std::string& profile, ID3DBlob** blob);
	std::vector<uint8_t> loadShaderFile(const std::string& csofile);

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
};

