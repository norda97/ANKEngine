#include "pch.h"
#include "Entity.h"

#include "Core/Model/Model.h"

Entity::Entity() : model(nullptr)
{
	transform = Matrix();
}

Entity::~Entity()
{
}

void Entity::setPosition(const Vector3& position)
{
	this->position = position;
	updateTransform();
}

void Entity::setRotation(const Vector3& rotation)
{
	this->rotation = rotation;
	updateTransform();
}

void Entity::setScale(const Vector3& scale)
{
	this->scale = scale;
	updateTransform();
}

Vector3 Entity::getPosition() const
{
	return this->position;
}

Vector3 Entity::getRotation() const
{
	return this->rotation;
}

Vector3 Entity::getScale() const
{
	return this->scale;
}

const Matrix& Entity::getTransform() const
{
	return transform;
}

void Entity::setModel(Model* model)
{
	if(this->model)
		model->removeInstance(this);

	this->model = model;

	model->addInstance(this);
}

const Model& Entity::getModel()  const
{
	return *this->model;
}

void Entity::updateTransform()
{
	this->transform = Matrix::CreateScale(this->scale);
	this->transform *= Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z);
	this->transform *= Matrix::CreateTranslation(this->position);
}
