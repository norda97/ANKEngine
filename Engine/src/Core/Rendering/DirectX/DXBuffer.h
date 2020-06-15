#pragma once

class DXBuffer
{
public:
	DXBuffer();
	~DXBuffer();

	bool init(const void* data, uint32_t size, D3D11_USAGE usage, uint32_t bufferType, uint32_t accessFlag);
	void transferData(void* data, uint32_t size, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD);

	ID3D11Buffer* getBuffer();
private:
	ID3D11Buffer* buffer;

};

