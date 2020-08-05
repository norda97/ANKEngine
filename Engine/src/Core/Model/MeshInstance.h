#pragma once

#include "Core/Model/ModelTypes.h"

//class Material;
//class Mesh;
//class Entity;

struct MeshInstance
{
	MeshID meshID;
	MaterialID materialID;
};

//class MeshInstance
//{
//public:
//	MeshInstance(Mesh* mesh, Material * material);
//	MeshInstance(const MeshInstance& other) noexcept;
//	MeshInstance(MeshInstance&& other) noexcept;
//	~MeshInstance();
//
//	MeshInstance& operator= (const MeshInstance& other);
//
//	void setMaterial(Material * material);
//	Material& getMaterial() const;
//	Material& makeMaterialUnique();
//
//	void setMesh(Mesh* mesh);
//	const Mesh& getMesh() const;
//
//	/*const std::list<const Entity*>& getInstanceList() const;
//	void addInstance(Entity* e);
//	void removeInstance(Entity* e);
//*/
//private:
//	void copy(const MeshInstance& other);
//
//	MeshID mesh;
//	MaterialID material;
//
//	//std::list<const Entity*> instances;
//
//};