#pragma once

#include "Core/Logic/Scenes/Scene.h"
#include "Core/Rendering/DirectX/DXRenderer.h"

#include "Core/ECS/EntityComponentSystem.h"

#include "Core/Utils/Cameras/Camera.h"

class RenderSystem;
class PhysicsSystem;
class HoverSystem;

class MainScene : public IScene
{
public:
	MainScene();
	~MainScene();

	bool Init();
	bool Update(float dt);
	bool Render();
	void Shutdown();

private:
	std::vector<Entity> m_Entities;

	RenderSystem* m_pRenderSystem;
	PhysicsSystem* m_pPhysicsSystem;
	HoverSystem* m_pHoverSystem;

	ECS m_Ecs;

	DXRenderer m_Renderer;
	Camera m_Camera;
};