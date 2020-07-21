#pragma once

class Model;

#include "Core/Model/Material.h"

class Entity
{
public:
	Entity();
	~Entity();

	void setPosition(const Vector3& position);
	void setRotation(const Vector3& rotation);
	void setScale(const Vector3& scale);

	Vector3 getPosition() const;
	Vector3 getRotation() const;
	Vector3 getScale() const;

	const Matrix& getTransform() const;

	void setModel(Model* model);
	const Model& getModel() const;

private:
	Model* model;

	void updateTransform();

	// temporary
	Matrix transform;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};