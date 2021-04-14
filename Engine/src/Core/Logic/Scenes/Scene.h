#pragma once

//#include "Core/Logic/EntityHandler.h"

struct alignas(16) SceneVariables
{
	Matrix VP;
	Vector3 camPos;
	float time;
};

class IScene
{
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual bool Init() = 0;
	virtual bool Update(float dt) = 0;
	virtual bool Render() = 0;
	virtual void Shutdown() = 0;

protected:
	//EntityHandler entityHandler;
};

