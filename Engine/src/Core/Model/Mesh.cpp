#include "pch.h"
#include "Mesh.h"

#include "Core/Model/Material.h"

Mesh::Mesh()
	: indexCount(0), hasIndices(false), hasNormals(false), hasTexCoords(false), hasTangents(false)
{

}

Mesh::~Mesh()
{

}

unsigned Mesh::getIndexCount() const
{
	return this->indexCount;
}

void Mesh::setIndexCount(unsigned count)
{
	this->indexCount = count;
}


const Buffer* Mesh::getVertexBuffer() const
{
	return this->vertexBuffer;
}

const Buffer* Mesh::getIndexBuffer() const
{
	return this->indexBuffer;
}

void Mesh::setVertexBuffer(Buffer* buffer)
{
	this->vertexBuffer = buffer;
}

void Mesh::setIndexBuffer(Buffer* buffer)
{
	this->indexBuffer = buffer;
}
