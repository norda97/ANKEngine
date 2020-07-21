#pragma once

class Scene;

class SceneHandler
{
public:
	SceneHandler();
	~SceneHandler();

	bool setCurrentScene(Scene* scene);

	void tick(double dt);


	void shutdown();
private:
	void shutdownCurrScene();


	Scene* currentScene;

};

