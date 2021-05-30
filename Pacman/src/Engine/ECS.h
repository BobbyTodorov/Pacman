#ifndef _ECS_HEADER_INCLUDED_
#define _ECS_HEADER_INCLUDED_

#include <iostream>
#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>
#include <set>
#include "Components.h"

using Entity = unsigned int;
const Entity MAX_ENTITIES = 5000;

using ComponentType = unsigned short;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;


class EntityManager
{
private:
	std::queue<Entity> availableEntities;
	std::array<Signature, MAX_ENTITIES> signatures;

	unsigned int entitiesCount;

public:
	EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			availableEntities.push(entity);
		}

		entitiesCount = 0;
	}

	Entity CreateEntity()
	{
		Entity id = availableEntities.front();
		availableEntities.pop();
		++entitiesCount;

		return id;
	}

	void DestroyEntity(const Entity& entity) 
	{
		signatures[entity].reset();

		availableEntities.push(entity);
		--entitiesCount;
	}

	void SetSignature(const Entity& entity, const Signature& signature)
	{
		signatures[entity] = signature;
	}

	Signature GetSignature(const Entity& entity)
	{
		return signatures[entity];
	}
};

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(const Entity& entity) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray
{
private:
	std::array<T, MAX_ENTITIES> componentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<Entity, size_t> entityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, Entity> indexToEntityMap;

	size_t validEntriesInArray;

public:
	void InsertData(const Entity& entity, T component)
	{
		size_t newIndex = validEntriesInArray;
		entityToIndexMap[entity] = newIndex;
		indexToEntityMap[newIndex] = entity;
		componentArray[newIndex] = component;
		++validEntriesInArray;
	}

	void RemoveData(const Entity& entity)
	{
		size_t indexOfRemovedEntity = entityToIndexMap[entity];
		size_t indexOfLastElement = validEntriesInArray - 1;
		componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

		Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
		entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		entityToIndexMap.erase(entity);
		indexToEntityMap.erase(indexOfLastElement);

		--validEntriesInArray;
	}

	bool HasData(const Entity& entity)
	{
		return indexToEntityMap[entityToIndexMap[entity]] == entity;
	}

	T& GetData(const Entity& entity)
	{
		return componentArray[entityToIndexMap[entity]];
	}

	void EntityDestroyed(const Entity& entity) override
	{
		if (entityToIndexMap.find(entity) != entityToIndexMap.end())
		{
			RemoveData(entity);
		}
	}
};

class ComponentManager
{
private:
	std::unordered_map<const char*, ComponentType> componentTypes;

	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};

	ComponentType nextComponentType{};

	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}

public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		componentTypes.insert({ typeName, nextComponentType });
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		++nextComponentType;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		return componentTypes[typeName];
	}

	template<typename T>
	void AddComponent(const Entity& entity, T component)
	{
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(const Entity& entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	bool HasComponent(const Entity& entity)
	{
		return GetComponentArray<T>()->HasData(entity);
	}

	template<typename T>
	T& GetComponent(const Entity& entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}

	void EntityDestroyed(const Entity& entity)
	{
		for (auto const& pair : componentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}
};

class System
{
public:
	std::set<Entity> entities;
};

class SystemManager
{
private:
	std::unordered_map<const char*, Signature> signatures{};

	std::unordered_map<const char*, std::shared_ptr<System>> systems{};

public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		auto system = std::make_shared<T>();
		systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(const Signature& signature)
	{
		const char* typeName = typeid(T).name();

		signatures.insert({ typeName, signature });
	}

	void EntityDestroyed(const Entity& entity)
	{
		for (auto const& pair : systems)
		{
			auto const& system = pair.second;

			system->entities.erase(entity);
		}
	}

	void EntitySignatureChanged(const Entity& entity, const Signature& entitySignature)
	{
		for (auto const& pair : systems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = signatures[type];

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
};

class Engine
{
private:
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<SystemManager> systemManager;

public:
	Engine() {}

	void Init()
	{
		componentManager = std::make_unique<ComponentManager>();
		entityManager = std::make_unique<EntityManager>();
		systemManager = std::make_unique<SystemManager>();
	}

	Entity CreateEntity()
	{
		Entity newEntity = entityManager->CreateEntity();
		
		return newEntity;
	}

	void DestroyEntity(const Entity& entity)
	{
		entityManager->DestroyEntity(entity);
		componentManager->EntityDestroyed(entity);
		systemManager->EntityDestroyed(entity);
	}
	
	template<typename T>
	void RegisterComponent()
	{
		componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(const Entity& entity, T component)
	{
		componentManager->AddComponent<T>(entity, component);

		auto signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), true);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(const Entity& entity)
	{
		componentManager->RemoveComponent<T>(entity);

		auto signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), false);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	bool HasComponent(const Entity& entity)
	{
		return componentManager->HasComponent<T>(entity);
	}

	template<typename T>
	T& GetComponent(const Entity& entity)
	{
		return componentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return componentManager->GetComponentType<T>();
	}


	
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return systemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(const Signature& signature)
	{
		systemManager->SetSignature<T>(signature);
	}
};

#endif // !_ECS_HEADER_INCLUDED_

