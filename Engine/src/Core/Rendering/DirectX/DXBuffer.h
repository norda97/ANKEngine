#pragma once

#include "Core/Rendering/General/Buffer.h"

class DXBuffer : public Buffer
{
public:
	DXBuffer();
	~DXBuffer();

	bool Init(const void* data, uint32_t size, D3D11_USAGE usage, uint32_t bufferType, uint32_t accessFlag);
	void update(void* data, uint32_t size, uint32_t offset, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD);

	/*
		Removes all current data
	*/
	void resize(uint32_t size);

	const ComPtr<ID3D11Buffer>& getBuffer() const;

	const uint32_t getSize() const;

private:
	bool initilized;
	uint32_t size;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	D3D11_BUFFER_DESC desc;
};

