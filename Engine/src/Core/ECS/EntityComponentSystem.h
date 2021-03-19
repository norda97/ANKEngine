#pragma once
#include <memory>

#include "Core/ECS/ComponentManager.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/SystemManager.h"
#include "Core/ECS/ECSTypes.h"

class ECS
{
public:
	void Init()
	{
		this->entityManager = std::make_unique<EntityManager>();
		this->componentManager = std::make_unique<ComponentManager>();
		this->systemManager = std::make_unique<SystemManager>();
	}

	Entity CreateEntity()
	{
		return this->entityManager->addEntity();
	}

	void DestroyEntity(Entity entity)
	{
		this->systemManager->entityDestroyed(entity);
		this->componentManager->entityDestroyed(entity);
		this->entityManager->removeEntity(entity);
	}

	template <typename T>
	void RegisterComponent()
	{
		this->componentManager->RegisterComponent<T>();
	}
	
	template <typename T>
	void AddComponent(Entity entity, T component)
	{
		this->componentManager->AddComponent<T>(entity, component);

		Signature signature = this->entityManager->getSignature(entity);
		signature.set(this->componentManager->GetComponentType<T>(), true);
		this->entityManager->setSignature(entity, signature);

		this->systemManager->entitySignatureChanged(entity, signature);
	}

	template <typename T>
	void RemoveComponent(Entity entity)
	{
		this->componentManager->RemoveComponent<T>(entity);

		Signature signature = this->entityManager->getSignature(entity);
		signature.set(this->componentManager->GetComponentType<T>(), false);
		this->entityManager->setSignature(entity, signature);
		
		this->systemManager->entitySignatureChanged(entity, signature);
	}

	template <typename T>
	bool HasComponent(Entity entity)
	{
		return this->componentManager->HasComponent<T>(entity);
	}

	template <typename T>
	T& GetComponent(Entity entity)
	{
		return this->componentManager->GetComponent<T>(entity);
	}

	template <typename T>
	ComponentType GetComponentType()
	{
		return this->componentManager->GetComponentType<T>();
	}

	template <typename T>
	T* RegisterSystem()
	{
		return this->systemManager->RegisterSystem<T>();
	}

	template <typename T>
	void SetSystemSignature(Signature signature)
	{
		this->systemManager->setSignature<T>(signature);
	}

private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;
};