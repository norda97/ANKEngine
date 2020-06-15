#include "pch.h"
#include "DXDeviceInstance.h"
#include "DXShader.h"

#include<fstream>

DXShader::DXShader() : vertexShader(NULL), pixelShader(NULL), layout(NULL)
{

}

DXShader::~DXShader()
{
	if (this->vertexShader)
		this->vertexShader->Release();

	if (this->pixelShader)
		this->pixelShader->Release();

	if (this->layout)
		this->layout->Release();
}

bool DXShader::init(const std::string& vFilename, const std::string& pFilename, std::vector<D3D11_INPUT_ELEMENT_DESC>& ied)
{

#if LOAD_PRECOMPILED_SHADERS
	auto vertexData = loadShaderFile(vFilename);
	auto pixelData = loadShaderFile(pFilename);

	DXDeviceInstance::get().getDev()->CreateVertexShader(vertexData.data(), vertexData.size(), NULL, &this->vertexShader);
	DXDeviceInstance::get().getDev()->CreatePixelShader(pixelData.data(), pixelData.size(), NULL, &this->pixelShader);

	DXDeviceInstance::get().getDev()->CreateInputLayout(ied.data(), 1, vertexData.data(), vertexData.size(), &this->layout);
#else // LOAD_PRECOMPILED_SHADERS

	ID3DBlob *vsBlob, *psBlob;
	
	assert(compileShader(ANK_SHADER_PATH + vFilename, "VSMain", "vs_4_0_level_9_1", &vsBlob));
	assert(compileShader(ANK_SHADER_PATH + pFilename, "PSMain", "ps_4_0_level_9_1", &psBlob));

	DXDeviceInstance::get().getDev()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->vertexShader);
	DXDeviceInstance::get().getDev()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &this->pixelShader);
	
	DXDeviceInstance::get().getDev()->CreateInputLayout(ied.data(), ied.size(), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->layout);

	vsBlob->Release();
	psBlob->Release();
#endif

	return true;
}

void DXShader::prepare()
{
	DXDeviceInstance::get().getDevCon()->VSSetShader(this->vertexShader, 0, 0);
	DXDeviceInstance::get().getDevCon()->PSSetShader(this->pixelShader, 0, 0);
	DXDeviceInstance::get().getDevCon()->IASetInputLayout(this->layout);
}

bool DXShader::compileShader(const std::string& file, const std::string& entry, const std::string& profile, ID3DBlob** blob)
{
	LPCWSTR fileWide = std::wstring(file.begin(), file.end()).c_str();


	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef MINI_DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* blobCode(nullptr), *blobError(nullptr);

	std::vector<uint8_t> srcData = loadShaderFile(file);
	HRESULT hr = D3DCompile(srcData.data(), srcData.size(), NULL, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), profile.c_str(), flags, 0, &blobCode, &blobError);

	if (FAILED(hr))
	{
		// Write error if blobeError has been filled
		if (blobError)
		{
			// Print error message and return
			std::string errMsg((char*)blobError->GetBufferPointer());

			ANK_ERROR(std::string(file + " - " + errMsg).c_str());
			blobError->Release();
			return false;
		}
		else 
		{
			throw std::invalid_argument("Failed to find file");
		}

		if (blobCode)
			blobCode->Release();
	}

	*blob = blobCode;
	return true;
}

std::vector<uint8_t> DXShader::loadShaderFile(const std::string& csofile)
{
	std::vector<uint8_t> data;

	std::ifstream stream(csofile, std::ios::in | std::ios::binary | std::ios::ate);

	if (stream.is_open())
	{
		int size = (int)stream.tellg();

		data.resize(size);
		stream.seekg(0, std::ios::beg);
		stream.read((char*)&data[0], size);
		stream.close();
	}

	return data;
}
