#pragma once

class Entity;
class Model;

class EntityHandler
{
public:
	EntityHandler();
	~EntityHandler();

	Entity* addEntity(const Vector3& positon, const Vector3& rotation, const Vector3& scale, Model* model);

	const std::vector<Entity*>& getEntities() const;

	void update(float dt);

private:
	std::vector<Entity*> entities;
};