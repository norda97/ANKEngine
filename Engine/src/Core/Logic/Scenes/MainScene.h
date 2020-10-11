#pragma once

#include "Core/Logic/Scenes/Scene.h"
#include "Core/Rendering/DirectX/DXRenderer.h"

#include "Core/ECS/EntityComponentSystem.h"

class RenderSystem;
class PhysicsSystem;
class HoverSystem;

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
	std::vector<Entity> entities;

	RenderSystem* renderSystem;
	PhysicsSystem* physicsSystem;
	HoverSystem* hoverSystem;

	ECS ecs;
};