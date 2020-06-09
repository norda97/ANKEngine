#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::init(float fov, float aspect, Vector3 target, Vector3 position, float nearPlane, float farPlane)
{
	this->view = Matrix::CreateLookAt(position, target, Vector3(0.f, 1.f, 0.f));
	this->projection = Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

	return true;
}

Matrix Camera::getView() const
{
	return this->view;
}

Matrix Camera::getProjection() const
{
	return this->projection;
}

Matrix Camera::getViewProjection() const
{
	return this->view * this->projection;
}
