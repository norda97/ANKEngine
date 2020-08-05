#include "pch.h"
#include "RenderSystem.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#include "Core/Model/ModelHandler.h"
#include "Core/Model/Model.h"
#include "Core/Model/MeshInstance.h"
#include "Core/Model/ModelTypes.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Drawable.h"

#include "Core/ECS/EntityComponentSystem.h"




void RenderSystem::init()
{
	this->instanceCount = entities.size();

	// Init camera
	this->camera.init(10.0f, XM_PI * 0.25f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), Vector3(0.0f), Vector3(0.f, 0.f, -30.0f), 0.1f, 1000.0f);

	this->renderer.init();
	this->renderer.setCamera(&camera);

	ANK_ASSERT(this->transformBuffer.init(NULL, sizeof(Instance) * MAX_MESH_INSTANCES, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE), "Failed to init constant buffer for transforms.")
}

void RenderSystem::update(ECS& ecs, float dt)
{
	// INEFFCIENT FIND BETTER SOLUTION
	for (auto & material : this->transformMap)
	{
		for (auto & pair : material.second)
		{
			auto& matrix = pair.second;
			matrix.clear();
		}
	}

	// Update camera
	camera.update(dt);

	// Update instance buffers
	auto const& modelMap = ModelHandler::get().getModels();

	for (auto const& entity : this->entities)
	{
		auto const& transform = ecs.getComponent<Transform>(entity);
		auto const& drawable = ecs.getComponent<Drawable>(entity);
		Model* model = modelMap.at(drawable.modelID);

		for (auto const& meshInstance : model->getMeshInstances())
		{
			Matrix matrix = Matrix::CreateScale(transform.scale);
			matrix *= Matrix::CreateFromYawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z);
			matrix *= Matrix::CreateTranslation(transform.position);

			this->transformMap[meshInstance.materialID][meshInstance.meshID].push_back(matrix);
		}
	}

	// Prepare instancebuffer
	updateInstanceBuffer();

	// Bind instance world position buffer
	unsigned int strides[1] = { sizeof(Matrix) };
	unsigned int offsets[1] = { 0 };
	ID3D11Buffer* bufferPointers[1] = { this->transformBuffer.getBuffer().Get() };

	DXDeviceInstance::get().getDevCon()->IASetVertexBuffers(1, 1, bufferPointers, strides, offsets);

	// Render scene
	this->renderer.prepare();
	unsigned instanceOffset = 0;
	for (auto& material : this->transformMap)
	{
		renderer.setMaterial(material.first);
		for (auto& pair : material.second)
		{
			auto const& meshID = pair.first;
			unsigned instanceCount = pair.second.size();
			renderer.render(meshID, pair.second.size(), instanceOffset);
			instanceOffset += instanceCount;
		}
	}

	renderer.finishFrame();
}

void RenderSystem::updateInstanceBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };


	DXDeviceInstance::get().getDevCon()->Map(this->transformBuffer.getBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto materials = ModelHandler::get().getMaterials();
	unsigned instanceIndex = 0;

	for (auto& material : this->transformMap)
	{
		for (auto& pair : material.second)
		{
			auto& matrix = pair.second;
			size_t instanceCount = matrix.size();
			memcpy((char*)mappedResource.pData + (instanceIndex * sizeof(Instance)), (void*)matrix.data(), sizeof(Instance) * instanceCount);
			instanceIndex += instanceCount;
		}
	}
	DXDeviceInstance::get().getDevCon()->Unmap(this->transformBuffer.getBuffer().Get(), 0);

	//DXDeviceInstance::get().getDevCon()->Map(this->instanceBuffer.getBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//auto materials = ModelHandler::get().getMaterials();
	//unsigned matCount = materials.size();
	//if (matCount > 0)
	//{
	//	// For each material, render all instances of models whichs use the material.
	//	unsigned instanceIndex = 0;
	//	for (unsigned i = 0; i < matCount; i++)
	//	{
	//		const Material* mat = materials[i];
	//		for (const MeshInstance* mesh : mat->getRenderList())
	//		{
	//			////EXPENSIVE! - Can be solved with single memcpy per entity list
	//			//const std::list<const Entity*>& instances = mesh->getInstanceList();
	//			//for (const Entity* entity : instances)
	//			//{
	//			//	memcpy((char*)mappedResource.pData + (instanceIndex++ * sizeof(Instance)), (void*)&entity->getTransform(), sizeof(Instance));
	//			//}
	//		}
	//	}
	//}

	//DXDeviceInstance::get().getDevCon()->Unmap(this->instanceBuffer.getBuffer().Get(), 0);
}
