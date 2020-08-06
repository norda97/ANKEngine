#pragma once

#include "Core/ECS/System.h"

class ECS;

class HoverSystem : public System
{
public:
	void init();

	void update(ECS& ecs, float dt);
private:
	float timer;
};