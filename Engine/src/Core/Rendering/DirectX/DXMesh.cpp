#include "pch.h"
#include "DXMesh.h"

DXMesh::DXMesh()
{

}

DXMesh::~DXMesh()
{
}

void DXMesh::setVertices(const std::vector<float>& vertices)
{
	if (vertices.size() > 0) {
		// Keep copy of data on CPU
		this->vertices.resize(vertices.size());
		for (int i = 0; i < vertices.size(); i++) {
			this->vertices[i] = vertices[i];
		}

		this->vertexBuffer.init(vertices.data(), vertices.size() * sizeof(float), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);
	}
}

void DXMesh::setIndices(const std::vector<unsigned int>& indices)
{
	if (indices.size() > 0) {
		this->usesIndices = true;

		// Keep copy of data on CPU
		this->indices.resize(indices.size());
		for (int i = 0; i < indices.size(); i++) {
			this->indices[i] = indices[i];
		}

		this->indexBuffer.init(indices.data(), indices.size() * sizeof(unsigned int), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0);
	}

}
