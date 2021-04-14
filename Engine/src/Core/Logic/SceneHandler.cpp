#include "pch.h"
#include "SceneHandler.h"
#include "Core/Logic/Scenes/Scene.h"
#include "Core/Model/ModelHandler.h"

SceneHandler::SceneHandler()
	: m_pCurrentScene(nullptr)
{
}

SceneHandler::~SceneHandler()
{
	this->Shutdown();
}

bool SceneHandler::SetCurrentScene(IScene* pscene)
{
	this->ShutdownCurrScene();

	if (pscene) {
		this->m_pCurrentScene = pscene;
		this->m_pCurrentScene->Init();
		return true;
	}

	return false;
}

void SceneHandler::Tick(double dt)
{
	if (this->m_pCurrentScene) {

		this->m_pCurrentScene->Update(dt);

		this->m_pCurrentScene->Render();
	}
	else {
		LOG_ERROR("No scene set as current in scene handler!");
	}

}

void SceneHandler::Shutdown()
{
	ShutdownCurrScene();

	ModelHandler::Get().shutdown();
}

void SceneHandler::ShutdownCurrScene()
{
	if (this->m_pCurrentScene) {
		this->m_pCurrentScene->Shutdown();
		delete this->m_pCurrentScene;
		this->m_pCurrentScene = nullptr;
	}
}
