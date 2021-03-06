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

bool MainScene::Init()
{
	m_Renderer.Init();
	
	// Init camera
	m_Camera.Init(10.0f, XM_PI * 0.25f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), Vector3(0.0f, 8.f, 0.f), Vector3(-30.f, 8.f, 0.0f), 0.1f, 1000.0f);
	m_Renderer.setCamera(&m_Camera);

	m_Ecs.Init();

	m_Ecs.RegisterComponent<Transform>();
	m_Ecs.RegisterComponent<Drawable>();
	m_Ecs.RegisterComponent<RigidBody>();
	m_Ecs.RegisterComponent<Gravity>();

	m_pHoverSystem = m_Ecs.RegisterSystem<HoverSystem>();
	{
		Signature signature;
		signature.set(m_Ecs.GetComponentType<Gravity>());
		signature.set(m_Ecs.GetComponentType<Transform>());
		signature.set(m_Ecs.GetComponentType<RigidBody>());
		m_Ecs.SetSystemSignature<HoverSystem>(signature);
	}

	m_pPhysicsSystem = m_Ecs.RegisterSystem<PhysicsSystem>();
	{
		Signature signature;
		signature.set(m_Ecs.GetComponentType<Transform>());
		signature.set(m_Ecs.GetComponentType<RigidBody>());
		m_Ecs.SetSystemSignature<PhysicsSystem>(signature);
	}

	m_pRenderSystem = m_Ecs.RegisterSystem<RenderSystem>();
	{
		Signature signature;
		signature.set(m_Ecs.GetComponentType<Transform>());
		signature.set(m_Ecs.GetComponentType<Drawable>());
		m_Ecs.SetSystemSignature<RenderSystem>(signature);
	}
	m_pRenderSystem->Init(&m_Ecs);
	ModelHandler& mh = ModelHandler::Get();
	Model& sponza = mh.loadModel(std::string(ANK_MODEL_PATH).append("SponzaPBR/"), "sponza.obj", "sponza");
	Model& sphere = mh.loadModel(std::string(ANK_MODEL_PATH).append("MatTest/"), "lowpoly_sphere_224tris.obj", "sphere");

	// Material Showcase
	uint32_t sphereCount = 5;
	uint32_t currEntityCount = m_Entities.size();
	uint32_t index = currEntityCount;
	m_Entities.resize(currEntityCount + 25U);
	const float scale = 2.0f;
	const float offset = 4.0f;
	for (unsigned i = 0; i < sphereCount; i++)
	{
		for (unsigned j = 0; j < sphereCount; j++)
		{
			Entity entity = m_Ecs.CreateEntity();
			m_Entities[index] = entity;
			Model& newSphere = mh.duplicateModel(sphere, "redSphere");
			MaterialID newMatID = mh.createMaterial(Vector4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f - (float)(j * (1.0f / sphereCount)), (float)(i * (1.0f / sphereCount)));
			newSphere.changeMeshMaterial(0, newMatID);
			
			Vec3 startPos = { 70.0f, offset, offset * sphereCount * 0.5f };
			startPos += { 0.f, j * offset, i * -offset};

			m_Ecs.AddComponent<Transform>(entity,
				Transform{
					startPos,
					{0.f, 0.f, 0.f},
					{ scale, scale, scale }
				});

			m_Ecs.AddComponent<Drawable>(entity,
				Drawable{
					newSphere.getModelID()
				});
		}
	}

	// Sphere entities
	
	Model& redSphere = mh.duplicateModel(sphere, "redSphere");
	Model& blueSphere = mh.duplicateModel(sphere, "blueSphere");
	{
		MaterialID newMatID = mh.createMaterial(Vector4(1.0f, 1.0f, 0.0f, 1.0f), 0.1f, 0.9f);
		redSphere.changeMeshMaterial(0, newMatID);

		newMatID = mh.createMaterial(Vector4(0.0f, 0.0f, 1.0f, 1.0f), 0.5f, 0.9f);
		blueSphere.changeMeshMaterial(0, newMatID);
	}

	m_Entities.resize(m_Entities.size() + 30);
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> randPos(-8.f, 8.f);
		std::uniform_real_distribution<float> randScale(0.5f, 1.f);


		ANKModelID materials[3] = { sphere.getModelID(), redSphere.getModelID(), blueSphere.getModelID() };
		unsigned index = 0;
		for (auto& entity : m_Entities) {
			const Vec3 offset = { randPos(generator), 15.f, randPos(generator) };
			entity = m_Ecs.CreateEntity();

			m_Ecs.AddComponent<Gravity>(entity,
				Gravity{
					{0.f, -2.8f, 0.f}
				});

			m_Ecs.AddComponent<RigidBody>(entity,
				RigidBody{
					{0.f, 0.f, 0.f},
					{0.f, 0.f, 0.f}
				});

			float scale = randScale(generator);
			m_Ecs.AddComponent<Transform>(entity,
				Transform{
					offset,
					{0.f, 0.f, 0.f},
					{scale, scale, scale}
				});

			m_Ecs.AddComponent<Drawable>(entity,
				Drawable{
					materials[index % 3]
				});

			index++;
		}
	}

	// Sponza scene
	Entity entity = m_Ecs.CreateEntity();

	m_Ecs.AddComponent<Transform>(entity,
		Transform{
			{0.f, 0.f, 0.f},
			{0.f, 0.f, 0.f},
			{0.1f, 0.1f, 0.1f}
		});

	m_Ecs.AddComponent<Drawable>(entity,
		Drawable{
			sponza.getModelID()
		});

	return true;
}

bool MainScene::Update(float dt)
{
	m_pHoverSystem->update(m_Ecs, dt);
	m_pPhysicsSystem->update(m_Ecs, dt);

	m_Camera.update(dt);

	return true;
}

bool MainScene::Render()
{
	m_Renderer.prepare();

	m_pRenderSystem->update(m_Renderer);

	m_Renderer.finishFrame();

	return true;
}

void MainScene::Shutdown()
{
}


