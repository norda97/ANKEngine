#include "pch.h"
#include "Camera.h"
#include "Core/IO/Input.h"


Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::init(float defaultSpeed, float fov, float aspect, Vector3 target, Vector3 position, float nearPlane, float farPlane)
{
	this->defaultSpeed = defaultSpeed;

	this->forward = target - position;
	this->forward.Normalize();

	float omega = atan2(this->forward.x, this->forward.z);
	this->rotation.y = omega;

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
	float speedFactor = defaultSpeed;
	float radPerFrame = XM_PI *0.5;

	if (Input::get().keyPressed(KEY_SHIFT)) {
		speedFactor *= 8.0;
		radPerFrame *= 4.0;
	}

	if (Input::get().keyPressed(KEY_LEFT))
		this->rotation.y += radPerFrame * dt;
	if (Input::get().keyPressed(KEY_RIGHT))
		this->rotation.y -= radPerFrame * dt;
	if (Input::get().keyPressed(KEY_UP))
		this->rotation.x = max(this->rotation.x - radPerFrame * dt, -XM_PI * 0.5);
	if (Input::get().keyPressed(KEY_DOWN))
		this->rotation.x = min(this->rotation.x + radPerFrame * dt, XM_PI * 0.5);

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

const Vector3& Camera::getPosition() const
{
	return this->position;
}

void Camera::rotate(Vector3 axis, float rad)
{
	if (axis == Vector3(1.0f, 0.0f, 0.0f))
		this->rotation.x = rad;
	if (axis == Vector3(0.0f, 1.0f, 0.0f))
		this->rotation.y = rad;
	if (axis == Vector3(0.0f, 0.0f, 1.0f))
		this->rotation.z = rad;
}

Matrix Camera::getView() const
{
	return this->view;
}

Matrix Camera::getProjection() const
{
	return this->projection;
}

Matrix Camera::getRotation() const
{
	Matrix xRotMat = Matrix::CreateRotationX(this->rotation.x);
	Matrix yRotMat = Matrix::CreateRotationY(this->rotation.y);
	//Matrix zRotMat = Matrix::CreateRotationZ(this->rotation.z);
	Matrix finalRotMat = xRotMat * yRotMat;
	Vector3 forward = XMVector3TransformCoord(this->defaultForward, finalRotMat);
	Vector3 right = XMVector3TransformCoord(this->defaultRight, finalRotMat);
	Vector3 up = this->forward.Cross(this->right);
	up.Normalize();

	return Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 0.0f), forward, this->up);
}

Matrix Camera::getViewProjection() const
{
	return this->view * this->projection;
}
