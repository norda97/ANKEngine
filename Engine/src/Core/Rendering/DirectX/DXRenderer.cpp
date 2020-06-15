#include "pch.h"
#include "DXRenderer.h"

DXRenderer::DXRenderer()
{

	// Create input layout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 2, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(Vector4) * 3, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, sizeof(Matrix), D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	this->testShader.init("Instanced_V.hlsl", "FlashingColor_P.hlsl", ied);
	this->instanceShader.init("Instanced_V.hlsl", "SimpleColor_P.hlsl", ied);
	this->constantBuffer.init(NULL, sizeof(SceneVariables), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0);
	this->instanceBuffer.init(instances, sizeof(Instance) * INSTANCE_COUNT, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);

	// Prepare instancing
	for (int i = 0; i < INSTANCE_COUNT; i++)
	{
		instances[i].position = Matrix::CreateTranslation(Vector3(0.5 * i, 0.5 * i, 0.5 * i));
		instances[i].color = Color(1.0f, 0.1f * i, 0.0f, 1.0f);
	}
}

DXRenderer::~DXRenderer()
{
}

bool DXRenderer::renderModel(const DXModel& model)
{
	return true;
}
