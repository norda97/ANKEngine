#pragma once

#include "Core/ECS/System.h"

class ECS;

class PhysicsSystem : public System
{
public:
	void Init();

	void update(ECS& ecs, float dt);

	void insertEntityEvent(Entity entity) {};
	void eraseEntityEvent(Entity entity) {};
private:
};