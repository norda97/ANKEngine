#pragma once
#include <set>
#include "Core/ECS/ECSTypes.h"

class System
{
public:
	virtual ~System() {};
	std::set<Entity> entities;
};