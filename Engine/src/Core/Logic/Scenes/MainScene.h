#pragma once

#include "Core/Logic/Scenes/Scene.h"

#include "Core/ECS/EntityComponentSystem.h"

class RenderSystem;

class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();

	bool init();
	bool update(float dt);
	bool render();
	void shutdown();

private:

	RenderSystem* renderSystem;

	ECS ecs;
};