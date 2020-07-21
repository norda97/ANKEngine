#pragma once

#include "Scene.h"
//#include "Core/Rendering/DirectX/DXShader.h"
//#include "Core/Rendering/DirectX/DXBuffer.h"
#include "Core/Rendering/DirectX/DXRenderer.h"

#include "Core/Utils/Cameras/Camera.h"

class EditorScene : public Scene
{
public:
	EditorScene();
	~EditorScene();

	bool init();
	bool update(float dt);
	bool render();
	void shutdown();

private:
	Camera camera;
	DXRenderer renderer;
};