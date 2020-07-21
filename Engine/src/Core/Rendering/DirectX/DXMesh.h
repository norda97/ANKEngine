#pragma once

#include "Core/Rendering/DirectX/DXBuffer.h"
#include "Core/Rendering/DirectX/DXTexture.h"
#include "Core/Model/Mesh.h"

class DXMesh : public Mesh
{
public:
	DXMesh();

	const DXBuffer* getVertexBuffer() const;
	const DXBuffer* getIndexBuffer() const;

	//void setVertices(const std::vector<VertexData>& vertices, bool hasNormals = true, bool hasTexCoords = true, bool hasTangents = true);
	//void setIndices(const std::vector<unsigned int>& indices);

	void setVertices(DXBuffer* buffer);
	void setIndices(DXBuffer* buffer);

private:
	DXBuffer*	vertexBuffer;
	DXBuffer*	indexBuffer;
};