#pragma once

#include "Core/Rendering/DirectX/DXBuffer.h"

#include "Core/Model/Mesh.h"

class DXMesh : public Mesh
{
public:
	DXMesh();
	~DXMesh();

	void setVertices(const std::vector<float>& vertices);
	void setIndices(const std::vector<unsigned int>& indices);

private:
	DXBuffer vertexBuffer;
	DXBuffer indexBuffer;
};