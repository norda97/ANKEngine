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
		this->m_StagingTransformBuffer.Init(
			NULL,
			sizeof(Instance) * MAX_MESH_INSTANCES,
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_WRITE),
		"Failed to init staging buffer for transforms."
	);

	ANK_ASSERT(
		this->m_TransformBuffer.Init(
			NULL,
			sizeof(Instance) * MAX_MESH_INSTANCES,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			0),
		"Failed to init constant buffer for transforms."
	);

	ANK_ASSERT(ecs != nullptr, "ECS must be a valid pointer in renderSystem");
}

void RenderSystem::update(DXRenderer& renderer)
{
	// Update instance buffers
	auto const& modelMap = ModelHandler::Get().getModels();

	for (auto const& entity : this->entities)
	{
		auto & transform = this->m_pEcs->GetComponent<Transform>(entity);
		auto const& drawable = this->m_pEcs->GetComponent<Drawable>(entity);

		const Model& model = modelMap.at(drawable.ModelID);
		for (auto const& meshInstance : model.getMeshInstances())
		{
			Matrix matrix = Matrix::CreateScale(transform.Scale);
			matrix *= Matrix::CreateFromYawPitchRoll(transform.Rotation.y, transform.Rotation.x, transform.Rotation.z);
			matrix *= Matrix::CreateTranslation(transform.Position);

			this->m_InstanceData[meshInstance.materialID][meshInstance.meshID].updateEntity(entity, matrix);
		}
	}

	// Prepare instancebuffer
	updateTransformBuffer();

	// Render scene
	renderer.prepare();

	auto& devcon = DXDeviceInstance::GetDevCon();

	unsigned int strides[1] = { sizeof(InstanceData) };
	unsigned int offsets[1] = { 0 };
	devcon->IASetVertexBuffers(1, 1, m_TransformBuffer.GetBuffer().GetAddressOf(), strides, offsets);

	unsigned instanceOffset = 0;
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& materialID : this->m_InstanceData)
	{
		renderer.setMaterial(materialID.first);
		for (auto& meshID : materialID.second)
		{
			auto& instanceVector = meshID.second;
			unsigned instanceCount = instanceVector.GetData().size();
			//renderer.render(meshID, pair.second.size(), instanceOffset);

			unsigned int strides[1] = { sizeof(VertexData) };
			unsigned int offsets[1] = { 0 };

			Mesh& mesh = *ModelHandler::Get().getMesh(meshID.first);

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
	auto const& drawable = this->m_pEcs->GetComponent<Drawable>(entity);

	auto const& modelMap = ModelHandler::Get().getModels();

	const Model& model = modelMap.at(drawable.ModelID);
	for (auto const& meshInstance : model.getMeshInstances())
	{
		auto& transformContainer = m_InstanceData[meshInstance.materialID][meshInstance.meshID];

		transformContainer.addEntity(entity);
	}
}

void RenderSystem::eraseEntityEvent(Entity entity)
{
	auto const& drawable = this->m_pEcs->GetComponent<Drawable>(entity);

	auto const& modelMap = ModelHandler::Get().getModels();

	const Model & model = modelMap.at(drawable.ModelID);
	for (auto const& meshInstance : model.getMeshInstances())
	{
		auto& transformContainer = m_InstanceData[meshInstance.materialID][meshInstance.meshID];

		transformContainer.removeEntity(entity);
	}
}

void RenderSystem::updateTransformBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	auto devcon = DXDeviceInstance::GetDevCon();

	HRESULT hr = devcon->Map(m_StagingTransformBuffer.GetBuffer().Get(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{ 
		unsigned instanceIndex = 0;
		for (auto& materialID : this->m_InstanceData)
		{
			for (auto& meshID : materialID.second)
			{
				auto& transformContainer = meshID.second;
				auto& transformVector = transformContainer.GetData();
				size_t instanceCount = transformVector.size();
				memcpy((char*)mappedResource.pData + (instanceIndex * sizeof(Instance)), (void*)transformVector.data(), sizeof(Instance) * instanceCount);
				instanceIndex += instanceCount;
			}
		}
	}
	devcon->Unmap(m_StagingTransformBuffer.GetBuffer().Get(), 0);

	// Prepare copy of staging transform to transform on GPU
	devcon->CopyResource(m_TransformBuffer.GetBuffer().Get(), m_StagingTransformBuffer.GetBuffer().Get());

}
