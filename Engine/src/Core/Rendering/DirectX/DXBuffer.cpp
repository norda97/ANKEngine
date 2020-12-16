#include "pch.h"
#include "DXBuffer.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

DXBuffer::DXBuffer() : buffer(NULL), initilized(false)
{
}

DXBuffer::~DXBuffer()
{
	//if(this->buffer)
	//	this->buffer->Release();
}

bool DXBuffer::Init(const void* data, uint32_t size, D3D11_USAGE usage, uint32_t bufferType, uint32_t accessFlag)
{
	if (!initilized)
	{
		this->size = size;
		// 16 byte alignment
		if (this->size % 16 != 0)
			this->size = 16 * (uint32_t)(ceilf(float(size) / 16.f));


		// Vertex buffer
		this->desc = { 0 };
		this->desc.Usage = usage;
		this->desc.ByteWidth = this->size;
		this->desc.BindFlags = bufferType;
		this->desc.CPUAccessFlags = accessFlag;
		this->desc.MiscFlags = 0;

		HRESULT hr;

		if (data) {
			D3D11_SUBRESOURCE_DATA bufferData = { 0 };
			bufferData.pSysMem = data;
			hr = DXDeviceInstance::Get().GetDev()->CreateBuffer(&this->desc, &bufferData, this->buffer.ReleaseAndGetAddressOf());
		}
		else
			hr = DXDeviceInstance::Get().GetDev()->CreateBuffer(&this->desc, NULL, this->buffer.ReleaseAndGetAddressOf());

		if (FAILED(hr))
			return false;

	return true;
	}
	LOG_ERROR("Buffer already initilized!");
	return false;
}

void DXBuffer::update(void* data, uint32_t size, uint32_t offset, D3D11_MAP mapType)
{
	D3D11_MAPPED_SUBRESOURCE ms = { 0 };
	const D3D11_BOX sDstBox = { offset, 0U, 0U, offset+size, 1U, 1U };
	DXDeviceInstance::Get().GetDevCon()->Map(this->buffer.Get(), NULL, mapType, NULL, &ms);
	memcpy((char*)ms.pData + offset, data, size);
	DXDeviceInstance::Get().GetDevCon()->Unmap(this->buffer.Get(), NULL);
}

const ComPtr<ID3D11Buffer>& DXBuffer::getBuffer() const
{
	return this->buffer;
}

const uint32_t DXBuffer::getSize() const
{
	return this->size;
}

void DXBuffer::resize(uint32_t size)
{
	this->size = size;
	// 16 byte alignment
	if (this->size % 16 != 0)
		this->size = 16 * (uint32_t)(ceilf(float(size) / 16.f));

	// Vertex buffer
	HRESULT hr;
	hr = DXDeviceInstance::Get().GetDev()->CreateBuffer(&this->desc, NULL, this->buffer.ReleaseAndGetAddressOf());

	ANK_ASSERT(FAILED(hr), "Failed to resize DXBuffer");
}
