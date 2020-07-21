#include "pch.h"
#include "MeshInstance.h"

#include "Core/Model/Material.h"
#include "Core/Model/ModelHandler.h"

MeshInstance::MeshInstance(Mesh* mesh, Material* material) : material(nullptr), mesh(nullptr)
{
	setMaterial(material);
	setMesh(mesh);
}

MeshInstance::MeshInstance(const MeshInstance& other) noexcept : MeshInstance(other.mesh, other.material)
{
}

MeshInstance::MeshInstance(MeshInstance&& other) noexcept : MeshInstance(other.mesh, other.material)
{
}

MeshInstance::~MeshInstance()
{
	if (this->material && this->mesh)
		this->material->removeMeshInstance(this);
}

MeshInstance& MeshInstance::operator=(const MeshInstance& other)
{
	copy(other);
	return *this;
}

void MeshInstance::setMaterial(Material* material)
{
	// Detach mesh from current material
	if (this->material && this->mesh)
		this->material->removeMeshInstance(this);

	// Attach mesh to new material
	if (material) {
		this->material = material;
		this->material->addMeshInstance(this);
	}
}

Material& MeshInstance::getMaterial() const
{
	return *this->material;
}

Material& MeshInstance::makeMaterialUnique()
{
	Material* material = ModelHandler::get().createMaterial(this->material);
	setMaterial(material);

	return *this->material;
}

void MeshInstance::setMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

const Mesh& MeshInstance::getMesh() const
{
	return *this->mesh;
}

const std::list<const Entity*>& MeshInstance::getInstanceList() const
{
	return this->instances;
}

void MeshInstance::addInstance(Entity* e)
{
	this->instances.push_back(e);
}

void MeshInstance::removeInstance(Entity* e)
{
	this->instances.remove(e);
}

void MeshInstance::copy(const MeshInstance& other)
{
	this->mesh = other.mesh;
	this->material = other.material;
}
