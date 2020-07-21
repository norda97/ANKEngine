#pragma once

class Material;
class Mesh;
class Entity;

class MeshInstance
{
public:
	MeshInstance(Mesh* mesh, Material * material);
	MeshInstance(const MeshInstance& other) noexcept;
	MeshInstance(MeshInstance&& other) noexcept;
	~MeshInstance();

	MeshInstance& operator= (const MeshInstance& other);

	void setMaterial(Material * material);
	Material& getMaterial() const;
	Material& makeMaterialUnique();

	void setMesh(Mesh* mesh);
	const Mesh& getMesh() const;

	const std::list<const Entity*>& getInstanceList() const;
	void addInstance(Entity* e);
	void removeInstance(Entity* e);

private:
	void copy(const MeshInstance& other);

	Mesh* mesh;
	Material* material;

	std::list<const Entity*> instances;
};