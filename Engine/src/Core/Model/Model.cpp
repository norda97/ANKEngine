#include "pch.h"
#include "Model.h"

#include "Core/Model/MeshInstance.h"

Model::Model(ANKModelID modelID) : id(modelID)
{
}

Model::~Model()
{

}

const std::vector<MeshInstance>& Model::getMeshInstances() const
{
	return this->meshInstances;
}

bool Model::addMeshInstance(MeshInstance mesh)
{
	this->meshInstances.push_back(mesh);

	return true;
}

void Model::changeMeshMaterial(uint32_t index, MaterialID matID)
{
	if (index >= 0 && index < this->meshInstances.size())
		this->meshInstances[index].materialID = matID;
	else
		LOG_WARNING("Change Mesh Material: Index out of range");
}

ANKModelID Model::getModelID() const
{
	return this->id;
}

void Model::setModelID(ANKModelID id)
{
	this->id = id;
}
