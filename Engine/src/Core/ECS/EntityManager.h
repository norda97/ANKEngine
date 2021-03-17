#pragma once
#include "pch.h"
#include "ECSTypes.h"

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	Entity addEntity();

	void removeEntity(Entity entity);

	void setSignature(Entity entity, Signature signature);

	Signature getSignature(Entity entity) const;
private:

	std::queue<Entity> unusedEntities;

	std::array<Signature, MAX_ENTITIES> signatures;
	
	uint32_t entityCount;
};