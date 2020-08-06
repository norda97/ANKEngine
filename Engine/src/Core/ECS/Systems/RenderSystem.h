#pragma once
#include "Core/Math/AnkMath.h"
#include "Core/ECS/System.h"
#include "Core/Utils/Cameras/Camera.h"

#include "Core/Rendering/DirectX/DXRenderer.h"

class ECS;

class RenderSystem : public System
{
public:
	void init();

	void update(ECS& ecs, float dt);

private:
	void updateInstanceBuffer();

	DXRenderer renderer;

	DXBuffer transformBuffer;
	Camera camera;
	std::map<MaterialID, std::map<MeshID, std::vector<Mat4>> > transformMap;
	size_t instanceCount;
};