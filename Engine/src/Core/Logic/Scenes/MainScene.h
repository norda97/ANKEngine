#pragma once

#include "Core/Logic/Scenes/Scene.h"
#include "Core/Rendering/DirectX/DXRenderer.h"

#include "Core/ECS/EntityComponentSystem.h"

#include "Core/Utils/Cameras/Camera.h"

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

	RenderSystem* renderSystem;
	PhysicsSystem* physicsSystem;
	HoverSystem* hoverSystem;

	ECS ecs;

	DXRenderer renderer;
	Camera camera;
};