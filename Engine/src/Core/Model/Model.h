#pragma once


class Entity;
class MeshInstance;

struct InstanceData
{
	Matrix transform;
};

class Model
{
public:
	Model();
	Model(const Model& other) noexcept;
	Model(Model&& other) noexcept;
	virtual ~Model();

	Model& operator= (const Model& other);

	const std::vector<MeshInstance*>& getMeshInstances() const;
	bool addMesh(MeshInstance* mesh);

	void addInstance(Entity* e);
	void removeInstance(Entity* e);

private:
	void deepCopy(const Model& other);


	std::vector<MeshInstance*> meshInstances;
};