#include "pch.h"
#include "DXMesh.h"

DXMesh::DXMesh()
{

}

const DXBuffer* DXMesh::getVertexBuffer() const
{
	return this->vertexBuffer;
}

const DXBuffer* DXMesh::getIndexBuffer() const
{
	return this->indexBuffer;
}

void DXMesh::setVertices(DXBuffer* buffer)
{
	this->vertexBuffer = buffer;
}

void DXMesh::setIndices(DXBuffer* buffer)
{
	this->indexBuffer = buffer;
}
//
//void DXMesh::setVertices(const std::vector<VertexData>& vertices, bool hasNormals, bool hasTexCoords, bool hasTangents)
//{
//	if (vertices.size() > 0) {
//		unsigned vertexSize = 3; // Position
//		if (hasNormals)
//			vertexSize += 3; // Normals
//		if (hasTangents)
//			vertexSize += 3; // Tangent
//		if (hasTexCoords)
//			vertexSize += 2; // texCoords
//
//		// Keep copy of data on CPU
//		this->vertices.resize(vertexSize * vertices.size());
//
//		for (int i = 0; i < vertices.size(); i++) {
//			unsigned offset = i * vertexSize;
//
//			this->vertices[offset + 0] = vertices[i].position.x;
//			this->vertices[offset + 1] = vertices[i].position.y;
//			this->vertices[offset + 2] = vertices[i].position.z;
//			offset += 3;
//
//			if (hasNormals)
//			{
//				this->vertices[offset + 0] = vertices[i].normal.x;
//				this->vertices[offset + 1] = vertices[i].normal.y;
//				this->vertices[offset + 2] = vertices[i].normal.z;
//				offset += 3;
//			}
//
//			if (hasTangents)
//			{
//				this->vertices[offset + 0] = vertices[i].tangent.x;
//				this->vertices[offset + 1] = vertices[i].tangent.y;
//				this->vertices[offset + 2] = vertices[i].tangent.z;
//				offset += 3;
//			}
//
//			if (hasTexCoords)
//			{
//				this->vertices[offset + 0] = vertices[i].texCoord.x;
//				this->vertices[offset + 1] = vertices[i].texCoord.y;
//			}
//		}
//
//		this->vertexBuffer.init(this->vertices.data(), this->vertices.size() * sizeof(float), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);
//	}
//}
//
//void DXMesh::setIndices(const std::vector<unsigned int>& indices)
//{
//	if (indices.size() > 0) {
//		this->hasIndices = true;
//
//		// Keep copy of data on CPU
//		this->indices.resize(indices.size());
//		for (int i = 0; i < indices.size(); i++) {
//			this->indices[i] = indices[i];
//		}
//
//		this->indexBuffer.init(indices.data(), indices.size() * sizeof(unsigned int), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0);
//	}
//}
