#include "pch.h"
#include "EntityManager.h"

#include "ECSTypes.h"

EntityManager::EntityManager() : entityCount(0)
{
	for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
		unusedEntities.push(entity);
	}
}

EntityManager::~EntityManager()
{
}

Entity EntityManager::addEntity()
{
	assert(this->entityCount < MAX_ENTITIES && "Entity max count reached.");

	Entity e = this->unusedEntities.front();
	this->unusedEntities.pop();

	this->entityCount++;
	
	return e;
}

void EntityManager::removeEntity(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	this->signatures[entity].reset();

	this->unusedEntities.push(entity);

	this->entityCount--;
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	this->signatures[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity) const
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	return this->signatures[entity];
}
