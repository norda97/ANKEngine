#include "pch.h"
#include "EditorScene.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"
#include "Core/Rendering/DirectX/DXModel.h"

#include "Core/Model/ModelHandler.h"
#include "Core/Model/MeshInstance.h"
#include "Core/Logic/Entity.h"


EditorScene::EditorScene()
{
}

EditorScene::~EditorScene()
{
}

bool EditorScene::init()
{

	// Load sponza model
	Model* sponza = ModelHandler::get().loadModel(std::string(ANK_MODEL_PATH).append("SponzaPBR/"), "sponza.obj", "sponza");
	Model* cube = ModelHandler::get().loadModel(std::string(ANK_MODEL_PATH).append("Cube/"), "cube.obj", "cube");
	Model* spheres = ModelHandler::get().loadModel(std::string(ANK_MODEL_PATH).append("MatTest/"), "sphere.obj", "spheres");
	
	unsigned sphereCount = 9;
	for (unsigned i = 0; i < sphereCount; i++)
	{
		for (unsigned j = 0; j < sphereCount; j++)
		{

			for (auto& meshInstance : spheres->getMeshInstances())
			{
				Material& mat = meshInstance->makeMaterialUnique();
				float rough = 1.0f - (float)(j * (1.0f / sphereCount));
				float metal = 1.0f - (float)(i * (1.0f / sphereCount));
				mat.setRoughness(rough);
				mat.setMetallicness(metal);
				mat.setAlbedo({ 1.0f, 0.0f, 0.0f, 1.0f});
			}

			this->entityHandler.addEntity({ j * -2.f, i * 2.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.0f, 1.0f, 1.0f }, spheres);

			spheres = ModelHandler::get().duplicateModel(spheres, std::string("spheres" + std::to_string(i) + std::to_string(j)));
		}
	}

	this->entityHandler.addEntity({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.1f, 0.1f, 0.1f }, sponza);

	// Init camera
	this->camera.init(10.0f, XM_PI * 0.25f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), Vector3(0.0f), Vector3(0.f, 0.f, -30.0f), 0.1f, 1000.0f);

	this->renderer.init();
	this->renderer.setCamera(&camera);

	return true;
}

bool EditorScene::update(float dt)
{
	camera.update(dt);
	this->renderer.update(dt);

	return true;
}

bool EditorScene::render()
{
	this->renderer.render();

	return true;
}

void EditorScene::shutdown()
{
}
