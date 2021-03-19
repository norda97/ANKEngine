#pragma once
#include "pch.h"
#include "Core/ECS/ECSTypes.h"
#include "Core/ECS/ComponentArray.h"

class ComponentManager
{
public:
	~ComponentManager() 
	{
		for (auto const& pair : this->componentArrays)
		{
			delete pair.second;
		}
	}

	template <typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		ANK_ASSERT(this->componentTypes.find(typeName) == this->componentTypes.end(), "Component already registered!");

		this->componentTypes.insert({ typeName, nextComponentType++ });

		this->componentArrays[typeName] = new ComponentArray<T>();
	}

	template <typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		ANK_ASSERT(this->componentTypes.find(typeName) != this->componentTypes.end(), "Component has not been registered!");

		return this->componentTypes[typeName];
	}

	template <typename T>
	void AddComponent(Entity entity, T component)
	{
		getComponentArray<T>()->insertData(entity, component);
	}

	template <typename T>
	void RemoveComponent(Entity entity)
	{
		getComponentArray<T>()->removeData(entity);
	}

	template <typename T>
	bool HasComponent(Entity entity)
	{
		return getComponentArray<T>()->HasData(entity);
	}

	template <typename T>
	T& GetComponent(Entity entity)
	{
		return getComponentArray<T>()->GetData(entity);
	}

	void entityDestroyed(Entity entity)
	{
		for (auto const& pair : this->componentArrays)
		{
			auto const& componentArray = pair.second;
			componentArray->entityDestroyed(entity);
		}
	}

private:
	std::unordered_map<const char*, ComponentType> componentTypes{};

	std::unordered_map<const char*, IComponentArray*> componentArrays{};

	ComponentType nextComponentType{};


	template<typename T>
	ComponentArray<T>* getComponentArray()
	{
		const char* typeName = typeid(T).name();

		ANK_ASSERT(componentTypes.find(typeName) != componentTypes.end(), "Component not registered before use.");

		return static_cast<ComponentArray<T>*>(componentArrays[typeName]);
	}
};