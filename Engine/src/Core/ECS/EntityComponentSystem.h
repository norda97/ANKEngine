#pragma once
#include <memory>

#include "Core/ECS/ComponentManager.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/SystemManager.h"
#include "Core/ECS/ECSTypes.h"

class ECS
{
public:
	void init()
	{
		this->entityManager = std::make_unique<EntityManager>();
		this->componentManager = std::make_unique<ComponentManager>();
		this->systemManager = std::make_unique<SystemManager>();
	}

	Entity createEntity()
	{
		return this->entityManager->addEntity();
	}

	void destroyEntity(Entity entity)
	{
		this->systemManager->entityDestroyed(entity);
		this->componentManager->entityDestroyed(entity);
		this->entityManager->removeEntity(entity);
	}

	template <typename T>
	void registerComponent()
	{
		this->componentManager->registerComponent<T>();
	}
	
	template <typename T>
	void addComponent(Entity entity, T component)
	{
		this->componentManager->addComponent<T>(entity, component);

		Signature signature = this->entityManager->getSignature(entity);
		signature.set(this->componentManager->getComponentType<T>(), true);
		this->entityManager->setSignature(entity, signature);

		this->systemManager->entitySignatureChanged(entity, signature);
	}

	template <typename T>
	void removeComponent(Entity entity)
	{
		this->componentManager->removeComponent<T>(entity);

		Signature signature = this->entityManager->getSignature(entity);
		signature.set(this->componentManager->getComponentType<T>(), false);
		this->entityManager->setSignature(entity, signature);
		
		this->systemManager->entitySignatureChanged(entity, signature);
	}

	template <typename T>
	T& getComponent(Entity entity)
	{
		return this->componentManager->getComponent<T>(entity);
	}

	template <typename T>
	ComponentType getComponentType()
	{
		return this->componentManager->getComponentType<T>();
	}

	template <typename T>
	T* registerSystem()
	{
		return this->systemManager->registerSystem<T>();
	}

	template <typename T>
	void setSystemSignature(Signature signature)
	{
		this->systemManager->setSignature<T>(signature);
	}

private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;
};