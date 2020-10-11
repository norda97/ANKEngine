#pragma once

//class Entity;
#include "MeshInstance.h"

struct InstanceData
{
	Matrix transform;
};

class Model
{
public:
	Model() {};
	Model(ModelID modelID);
	virtual ~Model();
	
	const std::vector<MeshInstance>& getMeshInstances() const;
	bool addMeshInstance(MeshInstance mesh);

	void changeMeshMaterial(MeshID meshID, MaterialID matID);

	ModelID getModelID() const;

	/*
		Do not change modelID, without knowing what you are doing
	*/
	void setModelID(ModelID id);

private:
	ModelID id;

	std::vector<MeshInstance> meshInstances;
};