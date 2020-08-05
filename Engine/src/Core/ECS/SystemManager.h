#pragma once
#include "pch.h"
#include "Core/ECS/ECSTypes.h"
#include "System.h"

class SystemManager
{
public:
	~SystemManager()
	{
		for (auto const& pair : this->systems)
		{
			delete pair.second;
		}
	}

	template <typename T>
	T* registerSystem()
	{
		const char* typeName = typeid(T).name();

		ANK_ASSERT(this->systems.find(typeName) == this->systems.end(), "System already registered");

		auto system = new T();
		this->systems[typeName] = system;

		return system;
	}

	template <typename T>
	void setSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		ANK_ASSERT(this->systems.find(typeName) != this->systems.end(), "System has not been registered");

		this->signatures[typeName] = signature;
	}

	void entityDestroyed(Entity entity)
	{
		for (auto const& pair : this->systems) 
		{
			System* system = pair.second;

			system->entities.erase(entity);
		}
	}

	void entitySignatureChanged(Entity entity, Signature entitySignature)
	{
		for (auto const& pair : this->systems)
		{
			const char* type = pair.first;
			System* system = pair.second;
			Signature& systemSignature = this->signatures[type];

			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->entities.insert(entity);
			}
			else
			{
				system->entities.erase(entity);
			}
		}
	}


private:
	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> signatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, System*> systems{};
};