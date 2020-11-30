#include "pch.h"
#include "Model.h"

#include "Core/Model/MeshInstance.h"

Model::Model(ModelID modelID) : id(modelID)
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
		ANK_WARNING("Change Mesh Material: Index out of range");
}

ModelID Model::getModelID() const
{
	return this->id;
}

void Model::setModelID(ModelID id)
{
	this->id = id;
}
