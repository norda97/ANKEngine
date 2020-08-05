#include "pch.h"
#include "MainScene.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#include "Core/Model/ModelHandler.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Drawable.h"

#include "Core/ECS/Systems/RenderSystem.h"


MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

bool MainScene::init()
{
	ecs.init();

	ecs.registerComponent<Transform>();
	ecs.registerComponent<Drawable>();

	this->renderSystem = ecs.registerSystem<RenderSystem>();
	{
		Signature signature;
		signature.set(ecs.getComponentType<Transform>());
		signature.set(ecs.getComponentType<Drawable>());
		ecs.setSystemSignature<RenderSystem>(signature);
	}

	this->renderSystem->init();

	ModelID sponza = ModelHandler::get().loadModel(std::string(ANK_MODEL_PATH).append("SponzaPBR/"), "sponza.obj", "sponza");

	Entity entity = ecs.createEntity();

	ecs.addComponent<Transform>(entity,
		Transform{
			Vec3(0.f, 0.f, 0.f),
			Vec3(0.f, 0.f, 0.f),
			Vec3(0.1f, 0.1f, 0.1f),
		});

	ecs.addComponent<Drawable>(entity,
		Drawable{
			sponza
		});

	return true;
}

bool MainScene::update(float dt)
{
	this->renderSystem->update(ecs, dt);
	return true;
}

bool MainScene::render()
{
	return true;
}

void MainScene::shutdown()
{
}


