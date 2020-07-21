#include "pch.h"
#include "DXModel.h"

DXModel::DXModel()
{
}

DXModel::~DXModel()
{

}

const std::vector<Mesh>& DXModel::getMeshes() const
{
	return this->meshes;
}

bool DXModel::addMesh(const Mesh& mesh)
{
	this->meshes.push_back(mesh);

	return true;
}
