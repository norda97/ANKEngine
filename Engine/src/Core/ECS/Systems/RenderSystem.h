#pragma once
#include "Core/Math/AnkMath.h"
#include "Core/Model/ModelTypes.h"

#include "Core/ECS/System.h"
#include "Core/ECS/ComponentArray.h"

#include "Core/Rendering/DirectX/DXRenderer.h"


class ECS;

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
		this->data.push_back(Mat4());
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

	void updateEntity(Entity entity, Mat4 transform)
	{
		this->data[this->entityToIndex[entity]] = transform;
	}

	std::vector<Mat4>& getData()
	{
		return this->data;
	}

private:
	size_t reservedSize;
	std::unordered_map<Entity, size_t>  entityToIndex;
	std::unordered_map<size_t, Entity>  indexToEntity;
	std::vector<Mat4> data;
};

class RenderSystem : public System
{
public:
	void init(ECS* ecs);

	void update(DXRenderer& renderer);

	void insertEntity(Entity entity);
	void eraseEntity(Entity entity);

private:
	void updateInstanceBuffer();

	ECS* ecs;

	std::map <MaterialID, std::unordered_map<MeshID, InstanceDataContainer>>	instanceData;
	//std::map <MaterialID, std::unordered_map<MeshID, DXBuffer>>					instanceBuffers;

	DXBuffer transformBuffer;
	std::map<MaterialID, std::map<MeshID, std::vector<Mat4>> > transformMap;
	size_t instanceCount;
};