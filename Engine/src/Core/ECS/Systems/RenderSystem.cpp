#include "pch.h"
#include "RenderSystem.h"

#include "Core/Rendering/DirectX/DXDeviceInstance.h"

#include "Core/Model/ModelHandler.h"
#include "Core/Model/Model.h"
#include "Core/Model/MeshInstance.h"
#include "Core/Model/ModelTypes.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Drawable.h"
#include "Core/ECS/Components/RigidBody.h"

#include "Core/ECS/EntityComponentSystem.h"
#include "Core/ECS/ECSTypes.h"




void RenderSystem::Init(ECS* ecs)
{
	this->m_pEcs = ecs;
	this->m_InstanceCount = entities.size();

	ANK_ASSERT(
		this->m_TransformBuffer.Init(
			NULL,
			sizeof(Instance) * MAX_MESH_INSTANCES,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			0),
		"Failed to init constant buffer for transforms."
	);

	ANK_ASSERT(
		this->m_TransformStagingBuffer.Init(
			NULL,
			sizeof(Instance) * MAX_MESH_INSTANCES,
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_WRITE),
		"Failed to init staging constant buffer for transforms."
	);

	ANK_ASSERT(ecs != nullptr, "ECS must be a valid pointer in renderSystem");
}

void RenderSystem::update(DXRenderer& renderer)
{

	// Update instance buffers
	auto const& modelMap = ModelHandler::get().getModels();

	for (auto const& entity : this->entities)
	{
		auto & transform = this->m_pEcs->getComponent<Transform>(entity);
		auto const& drawable = this->m_pEcs->getComponent<Drawable>(entity);

		const Model& model = modelMap.at(drawable.modelID);
		for (auto const& meshInstance : model.getMeshInstances())
		{
			Matrix matrix = Matrix::CreateScale(transform.scale);
			matrix *= Matrix::CreateFromYawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z);
			matrix *= Matrix::CreateTranslation(transform.position);

			this->instanceData[meshInstance.materialID][meshInstance.meshID].updateEntity(entity, matrix);
		}
	}

	// Prepare instancebuffer
	updateTransformBuffer();

	// Render scene
	renderer.prepare();

	auto& devcon = DXDeviceInstance::GetDevCon();

	//devcon->VSSetConstantBuffers(1, 1, m_TransformBuffer.GetBuffer().GetAddressOf());

	unsigned int strides[1] = { sizeof(InstanceData) };
	unsigned int offsets[1] = { 0 };
	devcon->IASetVertexBuffers(1, 1, m_TransformBuffer.GetBuffer().GetAddressOf(), strides, offsets);

	unsigned instanceOffset = 0;
	for (auto& materialID : this->instanceData)
	{
		renderer.setMaterial(materialID.first);
		for (auto& meshID : materialID.second)
		{
			auto& instanceVector = meshID.second;
			unsigned instanceCount = instanceVector.getData().size();
			//renderer.render(meshID, pair.second.size(), instanceOffset);


			unsigned int strides[1] = { sizeof(VertexData) };
			unsigned int offsets[1] = { 0 };

			Mesh& mesh = *ModelHandler::get().getMesh(meshID.first);

			ID3D11Buffer* bufferPointers[1] = { static_cast<const DXBuffer*>(mesh.getVertexBuffer())->GetBuffer().Get() };

			devcon->IASetVertexBuffers(0, 1, bufferPointers, strides, offsets);
			devcon->IASetIndexBuffer(static_cast<const DXBuffer*>(mesh.getIndexBuffer())->GetBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
			devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//DXDeviceInstance::GetDevCon()->RSSetState(rsWireframe);
			devcon->DrawIndexedInstanced(mesh.getIndexCount(), instanceCount, 0, 0, instanceOffset);

			instanceOffset += instanceCount;
		}
	}

}

void RenderSystem::insertEntityEvent(Entity entity)
{
	auto const& drawable = this->m_pEcs->getComponent<Drawable>(entity);

	auto const& modelMap = ModelHandler::get().getModels();

	const Model& model = modelMap.at(drawable.modelID);
	for (auto const& meshInstance : model.getMeshInstances())
	{
		auto& transformContainer = instanceData[meshInstance.materialID][meshInstance.meshID];

		transformContainer.addEntity(entity);
	}
}

void RenderSystem::eraseEntityEvent(Entity entity)
{
	auto const& drawable = this->m_pEcs->getComponent<Drawable>(entity);

	auto const& modelMap = ModelHandler::get().getModels();

	const Model & model = modelMap.at(drawable.modelID);
	for (auto const& meshInstance : model.getMeshInstances())
	{
		auto& transformContainer = instanceData[meshInstance.materialID][meshInstance.meshID];

		transformContainer.removeEntity(entity);
	}
}

void RenderSystem::updateTransformBuffer()
{
	auto devCon = DXDeviceInstance::GetDevCon();

	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	HRESULT hr = devCon->Map(this->m_TransformStagingBuffer.GetBuffer().Get(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
	
	if (SUCCEEDED(hr))
	{ 
		unsigned instanceIndex = 0;
		for (auto& materialID : this->instanceData)
		{
			for (auto& meshID : materialID.second)
			{
				auto& transformContainer = meshID.second;
				auto& transformVector = transformContainer.getData();
				size_t instanceCount = transformVector.size();
				memcpy((char*)mappedResource.pData + (instanceIndex * sizeof(Instance)), (void*)transformVector.data(), sizeof(Instance) * instanceCount);
				instanceIndex += instanceCount;
			}
		}
	}
	devCon->Unmap(this->m_TransformStagingBuffer.GetBuffer().Get(), 0);

	devCon->CopyResource(m_TransformBuffer.GetBuffer().Get(), m_TransformStagingBuffer.GetBuffer().Get());
}
