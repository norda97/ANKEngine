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
	Model(ANKModelID ModelID);
	virtual ~Model();
	
	const std::vector<MeshInstance>& getMeshInstances() const;
	bool addMeshInstance(MeshInstance mesh);

	void changeMeshMaterial(MeshID meshID, MaterialID matID);

	ANKModelID getModelID() const;

	/*
		Do not change ModelID, without knowing what you are doing
	*/
	void setModelID(ANKModelID id);

private:
	ANKModelID id;

	std::vector<MeshInstance> meshInstances;
};