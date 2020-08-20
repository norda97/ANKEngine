#pragma once

#include "Core/ECS/System.h"

class ECS;

class HoverSystem : public System
{
public:
	void init();

	void update(ECS& ecs, float dt);

	void insertEntityEvent(Entity entity) {};
	void eraseEntityEvent(Entity entity) {};
private:
	float timer;
};