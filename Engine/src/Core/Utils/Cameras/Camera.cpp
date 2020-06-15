#include "pch.h"
#include "Camera.h"
#include "Core/IO/Input.h"


Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::init(float fov, float aspect, Vector3 target, Vector3 position, float nearPlane, float farPlane)
{
	this->forward = target - position;
	this->forward.Normalize();

	this->right = this->forward.Cross(this->defaultUp);
	this->right.Normalize();

	this->position = position;
	this->up = this->defaultUp;
	this->view = Matrix::CreateLookAt(position, target, this->defaultUp);
	this->projection = Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

	return true;
}

void Camera::update(float dt)
{
	float speedFactor = 1.0;
	float radPerFrame = XM_PI *0.5;

	if (Input::get().keyPressed(KEY_SHIFT)) {
		speedFactor = 3.0;
		radPerFrame *= 3.0;
	}

	if (Input::get().keyPressed(KEY_LEFT))
		this->rotation.y += radPerFrame * dt;
	if (Input::get().keyPressed(KEY_RIGHT))
		this->rotation.y -= radPerFrame * dt;
	if (Input::get().keyPressed(KEY_UP))
		this->rotation.x -= radPerFrame * dt;
	if (Input::get().keyPressed(KEY_DOWN))
		this->rotation.x += radPerFrame * dt;

	//Matrix rotationMatrix;
	//rotationMatrix.CreateFromYawPitchRoll(this->rotation.x, this->rotation.y, this->rotation.z);
	Matrix xRotMat = Matrix::CreateRotationX(this->rotation.x);
	Matrix yRotMat = Matrix::CreateRotationY(this->rotation.y);
	//Matrix zRotMat = Matrix::CreateRotationZ(this->rotation.z);
	Matrix finalRotMat = xRotMat * yRotMat;
	this->forward = XMVector3TransformCoord(this->defaultForward, finalRotMat);
	this->right = XMVector3TransformCoord(this->defaultRight, finalRotMat);
	this->up = this->forward.Cross(this->right);
	this->up.Normalize();

	if(Input::get().keyPressed(KEY_D))
		this->position -= this->right * speedFactor * dt;
	if (Input::get().keyPressed(KEY_A))
		this->position += this->right * speedFactor * dt;
	if (Input::get().keyPressed(KEY_W))
		this->position += this->forward * speedFactor * dt;
	if (Input::get().keyPressed(KEY_S))
		this->position -= this->forward * speedFactor * dt;

	Vector3 target = this->position + this->forward;

	this->view = Matrix::CreateLookAt(this->position, target, this->up);
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
