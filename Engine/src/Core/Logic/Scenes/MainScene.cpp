#include "pch.h"
#include "MainScene.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#include "Core/Model/ModelHandler.h"
#include "Core/Model/Material.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Drawable.h"
#include "Core/ECS/Components/RigidBody.h"
#include "Core/ECS/Components/Gravity.h"

#include "Core/ECS/Systems/RenderSystem.h"
#include "Core/ECS/Systems/PhysicsSystem.h"
#include "Core/ECS/Systems/HoverSystem.h"


MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

bool MainScene::init()
{
	this->renderer.init();
	
	// Init camera
	this->camera.init(10.0f, XM_PI * 0.25f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), Vector3(0.0f, 8.f, 0.f), Vector3(-30.f, 8.f, 0.0f), 0.1f, 1000.0f);
	this->renderer.setCamera(&camera);

	ecs.init();

	ecs.registerComponent<Transform>();
	ecs.registerComponent<Drawable>();
	ecs.registerComponent<RigidBody>();
	ecs.registerComponent<Gravity>();

	this->hoverSystem = ecs.registerSystem<HoverSystem>();
	{
		Signature signature;
		signature.set(ecs.getComponentType<Gravity>());
		signature.set(ecs.getComponentType<Transform>());
		signature.set(ecs.getComponentType<RigidBody>());
		ecs.setSystemSignature<HoverSystem>(signature);
	}

	this->physicsSystem = ecs.registerSystem<PhysicsSystem>();
	{
		Signature signature;
		signature.set(ecs.getComponentType<Transform>());
		signature.set(ecs.getComponentType<RigidBody>());
		ecs.setSystemSignature<PhysicsSystem>(signature);
	}

	this->renderSystem = ecs.registerSystem<RenderSystem>();
	{
		Signature signature;
		signature.set(ecs.getComponentType<Transform>());
		signature.set(ecs.getComponentType<Drawable>());
		ecs.setSystemSignature<RenderSystem>(signature);
	}
	ModelHandler& mh = ModelHandler::get();
	Model& sponza = mh.loadModel(std::string(ANK_MODEL_PATH).append("SponzaPBR/"), "sponza.obj", "sponza");
	Model& sphere = mh.loadModel(std::string(ANK_MODEL_PATH).append("MatTest/"), "lowpoly_sphere_224tris.obj", "sphere");

	

	this->renderSystem->init(&this->ecs);


	// Material Showcase
	/*uint32_t sphereCount = 5;
	uint32_t currEntityCount = entities.size();
	uint32_t index = currEntityCount;
	this->entities.resize(currEntityCount + 25);
	const float scale = 2.0f;
	const float offset = 4.0f;
	for (unsigned i = 0; i < sphereCount; i++)
	{
		for (unsigned j = 0; j < sphereCount; j++)
		{
			Entity entity = ecs.createEntity();
			this->entities[index] = entity;
			Model& newSphere = mh.duplicateModel(sphere, "redSphere");
			MaterialID newMatID = mh.createMaterial(Vector4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f - (float)(j * (1.0f / sphereCount)), (float)(i * (1.0f / sphereCount)));
			newSphere.changeMeshMaterial(0, newMatID);
			
			Vec3 startPos = { 0.0f, offset, offset * sphereCount * 0.5f };
			startPos += { 0.f, j * offset, i * -offset};

			ecs.addComponent<Transform>(entity,
				Transform{
					startPos,
					{0.f, 0.f, 0.f},
					{ scale, scale, scale }
				});

			ecs.addComponent<Drawable>(entity,
				Drawable{
					newSphere.getModelID()
				});
		}
	}*/

	// Sphere entities
	
	Model& redSphere = mh.duplicateModel(sphere, "redSphere");
	Model& blueSphere = mh.duplicateModel(sphere, "blueSphere");
	{
		MaterialID newMatID = mh.createMaterial(Vector4(1.0f, 1.0f, 0.0f, 1.0f), 0.1f, 0.9f);
		redSphere.changeMeshMaterial(0, newMatID);

		newMatID = mh.createMaterial(Vector4(0.0f, 0.0f, 1.0f, 1.0f), 0.5f, 0.9f);
		blueSphere.changeMeshMaterial(0, newMatID);
	}

	this->entities.resize(entities.size() + 30);
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> randPos(-8.f, 8.f);
		std::uniform_real_distribution<float> randScale(0.5f, 1.f);

		ANKModelID materials[3] = { sphere.getModelID(), redSphere.getModelID(), blueSphere.getModelID() };
		unsigned index = 0;
		for (auto& entity : entities) {
			entity = ecs.createEntity();

			ecs.addComponent<Gravity>(entity,
				Gravity{
					{0.f, -2.8f, 0.f}
				});

			ecs.addComponent<RigidBody>(entity,
				RigidBody{
					{0.f, 0.f, 0.f},
					{0.f, 0.f, 0.f}
				});

			float scale = randScale(generator);
			ecs.addComponent<Transform>(entity,
				Transform{
					{randPos(generator), 15.f, randPos(generator)},
					{0.f, 0.f, 0.f},
					{scale, scale, scale}
				});

			ecs.addComponent<Drawable>(entity,
				Drawable{
					materials[index % 3]
				});

			index++;
		}
	}

	// Sponza scene
	Entity entity = ecs.createEntity();

	ecs.addComponent<Transform>(entity,
		Transform{
			{0.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{0.1f, 0.1f, 0.1f}
		});

	ecs.addComponent<Drawable>(entity,
		Drawable{
			sponza.getModelID()
		});

	return true;
}

bool MainScene::update(float dt)
{
	this->hoverSystem->update(ecs, dt);
	this->physicsSystem->update(ecs, dt);

	camera.update(dt);

	return true;
}

bool MainScene::render()
{
	this->renderer.prepare();

	this->renderSystem->update(this->renderer);

	this->renderer.finishFrame();
	
	return true;
}

void MainScene::shutdown()
{
}


