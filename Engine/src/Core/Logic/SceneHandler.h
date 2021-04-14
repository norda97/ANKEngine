#pragma once

class IScene;

class SceneHandler
{
public:
	SceneHandler();
	~SceneHandler();

	bool SetCurrentScene(IScene* pscene);

	void Tick(double dt);


	void Shutdown();
private:
	void ShutdownCurrScene();

	IScene* m_pCurrentScene;
};

