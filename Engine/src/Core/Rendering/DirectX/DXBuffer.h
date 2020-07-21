#pragma once

#include "Core/Rendering/General/Buffer.h"

class DXBuffer : public Buffer
{
public:
	DXBuffer();
	~DXBuffer();

	bool init(const void* data, uint32_t size, D3D11_USAGE usage, uint32_t bufferType, uint32_t accessFlag);
	void update(void* data, uint32_t size, uint32_t offset, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD);

	const ComPtr<ID3D11Buffer>& getBuffer() const;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

