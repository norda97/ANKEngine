#pragma once
#include <set>
#include "Core/ECS/ECSTypes.h"

class System
{
public:
	virtual ~System() {};

	virtual void insertEntity(Entity entity) { this->entities.insert(entity); };
	virtual void eraseEntity(Entity entity) { this->entities.erase(entity); };

	std::set<Entity> entities;
};