#pragma once

#include "DXModel.h"
#include "DXShader.h"



struct alignas(16) SceneVariables
{
	Matrix VP;
	float time;
};

struct Instance
{
	Matrix position;
	Color color;
};

#define INSTANCE_COUNT 10
Instance instances[INSTANCE_COUNT];

class DXRenderer
{
public:
	DXRenderer();
	~DXRenderer();

	bool renderModel(const DXModel& model);

private:

	ID3D11RasterizerState* rsNormal = nullptr;
	ID3D11RasterizerState* rsWireframe = nullptr;

	DXShader testShader;
	DXShader instanceShader;
	
	DXBuffer constantBuffer;
	DXBuffer instanceBuffer;
};