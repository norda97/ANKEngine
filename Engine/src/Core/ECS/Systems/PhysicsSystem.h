#pragma once

#include "Core/ECS/System.h"

class ECS;

class PhysicsSystem : public System
{
public:
	void init();

	void update(ECS& ecs, float dt);
private:
};