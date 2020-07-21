#include "pch.h"
#include "EntityHandler.h"
#include "Entity.h"

#include "Core/Model/Model.h"

EntityHandler::EntityHandler()
{
}

EntityHandler::~EntityHandler()
{
	for (int i = 0; i < this->entities.size(); i++)
		delete this->entities[i];
}

Entity* EntityHandler::addEntity(const Vector3& positon, const Vector3& rotation, const Vector3& scale, Model* model)
{
	Entity* e = new Entity();

	e->setPosition(positon);
	e->setRotation(rotation);
	e->setScale(scale);

	this->entities.push_back(e);

	if (model)
		e->setModel(model);

	return e;
}

const std::vector<Entity*>& EntityHandler::getEntities() const
{
	return this->entities;
}

void EntityHandler::update(float dt)
{
}
