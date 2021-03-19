#pragma once

#include "Core/ECS/System.h"

class ECS;

class HoverSystem : public System
{
public:
	void Init();

	void update(ECS& ecs, float dt);

	void insertEntityEvent(Entity entity) {};
	void eraseEntityEvent(Entity entity) {};
private:
	float timer;
};