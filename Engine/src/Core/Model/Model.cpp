#include "pch.h"
#include "Model.h"

#include "Core/Model/MeshInstance.h"

Model::Model()
{
}

//Model::Model(const Model& other)  noexcept
//{
//	deepCopy(other);
//}
//
//Model::Model(Model&& other) noexcept
//{
//	deepCopy(other);
//}

Model::~Model()
{
	/*for (auto meshInstance : this->meshInstances) {
		if (meshInstance) {
			delete meshInstance;
			meshInstance = nullptr;
		}
	}
	this->meshInstances.clear();*/
}

//Model& Model::operator=(const Model& other)
//{
//	deepCopy(other);
//	return *this;
//}

//void Model::addInstance(Entity* e)
//{
//	for (unsigned i = 0; i < this->meshInstances.size(); i++)
//	{
//		this->meshInstances[i]->addInstance(e);
//	}
//}
//
//void Model::removeInstance(Entity* e)
//{
//	for (unsigned i = 0; i < this->meshInstances.size(); i++)
//	{
//		this->meshInstances[i]->removeInstance(e);
//	}
//}
//
//void Model::deepCopy(const Model& other)
//{
//	unsigned meshInstanceCount = other.meshInstances.size();
//	this->meshInstances.resize(meshInstanceCount);
//	for (unsigned i = 0; i < meshInstanceCount; i++)
//	{
//		this->meshInstances[i] = new MeshInstance(*other.meshInstances[i]);
//	}
//}

const std::vector<MeshInstance>& Model::getMeshInstances() const
{
	return this->meshInstances;
}

bool Model::addMeshInstance(MeshInstance mesh)
{
	this->meshInstances.push_back(mesh);

	return true;
}
