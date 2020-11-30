#include "pch.h"
#include "SceneHandler.h"
#include "Core/Logic/Scenes/Scene.h"
#include "Core/Model/ModelHandler.h"

SceneHandler::SceneHandler()
	: currentScene(nullptr)
{
}

SceneHandler::~SceneHandler()
{
	this->shutdown();
}

bool SceneHandler::setCurrentScene(Scene* scene)
{
	this->shutdownCurrScene();

	if (scene) {
		this->currentScene = scene;
		this->currentScene->init();
		return true;
	}

	return false;
}

void SceneHandler::tick(double dt)
{
	if (this->currentScene) {

		this->currentScene->update(dt);

		this->currentScene->render();
	}
	else {
		ANK_ERROR("No scene set as current in scene handler!\n");
	}

}

void SceneHandler::shutdown()
{
	shutdownCurrScene();

	ModelHandler::get().shutdown();
}

void SceneHandler::shutdownCurrScene()
{
	if (this->currentScene) {
		this->currentScene->shutdown();
		delete this->currentScene;
		this->currentScene = nullptr;
	}
}
