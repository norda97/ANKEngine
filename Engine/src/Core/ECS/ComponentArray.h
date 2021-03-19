#pragma once
#include"pch.h"
#include "ECSTypes.h"

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
public:

	void insertData(Entity entity, T component)
	{
		ANK_ASSERT(this->entityToIndexMap.find(entity) == this->entityToIndexMap.end(), "Component is already attached to Entity!");

		size_t newIndex = this->size;
		this->entityToIndexMap[entity] = newIndex;
		this->indexToEntityMap[newIndex] = entity;
		this->componentArray[newIndex] = component;

		this->size++;
	};

	void removeData(Entity entity)
	{

		ANK_ASSERT(this->entityToIndexMap.find(entity) != this->entityToIndexMap.end(), "Component is not attached to Entity!");

		size_t rmIndex = this->entityToIndexMap[entity];
		size_t indexOfLast = this->size - 1;
		this->componentArray[rmIndex] = this->componentArray[indexOfLast];

		size_t lastEntity = this->indexToEntityMap[indexOfLast];
		this->entityToIndexMap[lastEntity] = rmIndex; // Replace last entity index with removed entitys index
		this->indexToEntityMap[rmIndex] = lastEntity;

		this->entityToIndexMap.erase(entity);
		this->indexToEntityMap.erase(indexOfLast);

		this->size--;
	};

	bool HasData(Entity entity)
	{
		return this->entityToIndexMap.find(entity) != this->entityToIndexMap.end();
	}

	T& GetData(Entity entity)
	{
		ANK_ASSERT(this->entityToIndexMap.find(entity) != this->entityToIndexMap.end(), "Component is not attached to Entity!");

		return this->componentArray[this->entityToIndexMap[entity]];
	};

	void entityDestroyed(Entity entity) override
	{
		if (this->entityToIndexMap.find(entity) != this->entityToIndexMap.end())
		{
			removeData(entity);
		}
	};

private:
	std::array<T, MAX_ENTITIES> componentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<Entity, size_t> entityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, Entity> indexToEntityMap;

	size_t size;
};