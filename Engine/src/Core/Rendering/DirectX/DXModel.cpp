#include "pch.h"
#include "DXModel.h"

DXModel::DXModel()
{
}

DXModel::~DXModel()
{
}

bool DXModel::addMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
{
	DXMesh* mesh = new DXMesh;

	mesh->setVertices(vertices);
	mesh->setIndices(indices);

	this->meshes.push_back(std::make_shared<DXMesh>(mesh));

	return true;
}
