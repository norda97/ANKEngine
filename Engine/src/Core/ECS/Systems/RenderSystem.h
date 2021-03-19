#pragma once
#include "Core/Math/AnkMath.h"
#include "Core/Model/ModelTypes.h"

#include "Core/ECS/System.h"
#include "Core/ECS/ComponentArray.h"

#include "Core/Rendering/DirectX/DXRenderer.h"

#include "Core/Model/MaterialProperties.h"

class ECS;

template <typename T>
class InstanceDataContainer
{
public:
	InstanceDataContainer() : reservedSize(10)
	{
		this->data.reserve(reservedSize);
	}

	void addEntity(Entity entity)
	{
		unsigned index = entityToIndex.size();
		if (index >= this->reservedSize)
		{
			this->reservedSize *= 2U;
			this->data.reserve(this->reservedSize);
		}
		this->entityToIndex[entity] = index;
		this->indexToEntity[index] = entity;
		this->data.push_back(T());
	}

	void removeEntity(Entity entity)
	{

		Entity lastEntity = this->indexToEntity[indexToEntity.size() - 1];
		size_t lastIndex = this->entityToIndex[lastEntity];
		size_t rmIndex = this->entityToIndex[entity];
		
		this->data[rmIndex] = this->data[lastIndex];
		this->data.pop_back();

		this->entityToIndex[lastEntity] = rmIndex;
		this->indexToEntity[rmIndex] = lastEntity;

		this->entityToIndex.erase(entity);
		this->indexToEntity.erase(lastIndex);
	}

	void updateEntity(Entity entity, T transform)
	{
		this->data[this->entityToIndex[entity]] = transform;
	}

	std::vector<T>& GetData()
	{
		return this->data;
	}

private:
	size_t reservedSize;
	std::unordered_map<Entity, size_t>  entityToIndex;
	std::unordered_map<size_t, Entity>  indexToEntity;
	std::vector<T> data;
};

class RenderSystem : public System
{
public:
	void Init(ECS* ecs);

	void update(DXRenderer& renderer);

	void insertEntityEvent(Entity entity);
	void eraseEntityEvent(Entity entity);
private:
	void updateTransformBuffer();

	ECS* ecs;

	std::map <MaterialID, std::unordered_map<MeshID, InstanceDataContainer<Mat4>>>	instanceData;

	DXBuffer m_TransformBuffer;
	DXBuffer m_StagingTransformBuffer;
	size_t instanceCount;
};