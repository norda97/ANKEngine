#pragma once
#include <set>
#include "Core/ECS/ECSTypes.h"

class System
{
public:
	virtual ~System() {};

	virtual void insertEntityEvent(Entity entity) = 0;
	virtual void eraseEntityEvent(Entity entity) = 0;

	void insertEntity(Entity entity) { this->entities.insert(entity); insertEntityEvent(entity); };
	void eraseEntity(Entity entity) {
		if(this->entities.erase(entity))
			eraseEntityEvent(entity); 
	};


	std::set<Entity> entities;
};