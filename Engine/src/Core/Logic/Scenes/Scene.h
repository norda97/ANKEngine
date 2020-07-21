#pragma once

#include "Core/Logic/EntityHandler.h"

struct alignas(16) SceneVariables
{
	Matrix VP;
	Vector3 camPos;
	float time;
};

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual bool init() = 0;
	virtual bool update(float dt) = 0;
	virtual bool render() = 0;
	virtual void shutdown() = 0;


protected:
	EntityHandler entityHandler;
};

