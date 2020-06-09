#include "pch.h"
#include "DXBuffer.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

DXBuffer::DXBuffer() : buffer(NULL)
{
}

DXBuffer::~DXBuffer()
{
	if(this->buffer)
		this->buffer->Release();
}

bool DXBuffer::init(const void* data, uint32_t size, D3D11_USAGE usage, uint32_t bufferType, uint32_t accessFlag)
{
	// 16 byte alignment
	uint32_t alignedSize = size;
	if(size % 16 != 0)
		alignedSize = 16 * (uint32_t)(ceilf(float(size) / 16.f));

	// Vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = usage;                
	bd.ByteWidth = alignedSize;
	bd.BindFlags = bufferType;
	bd.CPUAccessFlags = accessFlag;
	bd.MiscFlags = 0;

	HRESULT hr;

	if (data) {
		D3D11_SUBRESOURCE_DATA bufferData = { 0 };
		bufferData.pSysMem = data;
		hr = DXDeviceInstance::get().getDev()->CreateBuffer(&bd, &bufferData, &this->buffer);
	}
	else 
		hr = DXDeviceInstance::get().getDev()->CreateBuffer(&bd, NULL, &this->buffer);

	if (FAILED(hr))
		return false;

	return true;
}

void DXBuffer::transferData(void* data, uint32_t size, D3D11_MAP mapType)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	DXDeviceInstance::get().getDevCon()->Map(this->buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, data, size);	
	DXDeviceInstance::get().getDevCon()->Unmap(this->buffer, NULL);
}

ID3D11Buffer* DXBuffer::getBuffer()
{
	return this->buffer;
}
