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
	Shutdown();
}

bool SceneHandler::SetCurrentScene(IScene* pscene)
{
	ShutdownCurrScene();

	if (pscene) {
		m_pCurrentScene = pscene;
		m_pCurrentScene->Init();
		return true;
	}

	return false;
}

void SceneHandler::Tick(double dt)
{
	if (m_pCurrentScene) {

		m_pCurrentScene->Update(dt);

		m_pCurrentScene->Render();
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
	if (m_pCurrentScene) {
		m_pCurrentScene->Shutdown();
		delete m_pCurrentScene;
		m_pCurrentScene = nullptr;
	}
}
