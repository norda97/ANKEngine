#include "pch.h"
#include "Camera.h"
#include "Core/IO/Input.h"


Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::Init(float defaultSpeed, float fov, float aspect, Vector3 target, Vector3 position, float nearPlane, float farPlane)
{
	m_DefaultSpeed = defaultSpeed;

	m_Forward = target - position;
	m_Forward.Normalize();

	float omega = atan2(m_Forward.x, m_Forward.z);
	m_Rotation.y = omega;

	m_Right = m_Forward.Cross(m_DefaultUp);
	m_Right.Normalize();

	m_Position = position;
	m_Up = m_DefaultUp;
	m_View = Matrix::CreateLookAt(position, target, m_DefaultUp);
	m_Projection = Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

	m_Interia = 0.9f;
	m_RotationLag = Vec3(0.0f);

	return true;
}

void Camera::update(float dt)
{
	float speedFactor = m_DefaultSpeed;
	float radPerFrame = XM_PI * 0.1f;

	if (Input::Get().keyPressed(KEY_SHIFT)) {
		speedFactor *= 8.0f;
		radPerFrame *= 2.0f;
	}

	if (Input::Get().keyPressed(KEY_LEFT))
		m_RotationLag.y += radPerFrame * dt;
	if (Input::Get().keyPressed(KEY_RIGHT))
		m_RotationLag.y -= radPerFrame * dt;
	if (Input::Get().keyPressed(KEY_UP))
		m_RotationLag.x -= radPerFrame * dt;
	if (Input::Get().keyPressed(KEY_DOWN))
		m_RotationLag.x += radPerFrame * dt;

	// Add smoothing
	m_Rotation += m_RotationLag;
	m_RotationLag *= m_Interia;

	// Constrain X axis
	if (m_Rotation.x < 0.f)
		m_Rotation.x = max(m_Rotation.x, -XM_PI * 0.5f);
	else
		m_Rotation.x = min(m_Rotation.x, XM_PI * 0.5f);

	//Matrix rotationMatrix;
	//rotationMatrix.CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z);
	Matrix xRotMat = Matrix::CreateRotationX(m_Rotation.x);
	Matrix yRotMat = Matrix::CreateRotationY(m_Rotation.y);
	//Matrix zRotMat = Matrix::CreateRotationZ(rotation.z);
	Matrix finalRotMat = xRotMat * yRotMat;
	m_Forward = XMVector3TransformCoord(m_DefaultForward, finalRotMat);
	m_Right = XMVector3TransformCoord(m_DefaultRight, finalRotMat);
	m_Up = m_Forward.Cross(m_Right);
	m_Up.Normalize();

	if (Input::Get().keyPressed(KEY_W))
		m_Position += m_Forward * speedFactor * dt;
	if (Input::Get().keyPressed(KEY_S))
		m_Position -= m_Forward * speedFactor * dt;
	if (Input::Get().keyPressed(KEY_A))
		m_Position += m_Right * speedFactor * dt;
	if(Input::Get().keyPressed(KEY_D))
		m_Position -= m_Right * speedFactor * dt;
	if (Input::Get().keyPressed(KEY_SPACE))
		m_Position.y += speedFactor * dt;
	if (Input::Get().keyPressed(KEY_CTRL))
		m_Position.y -= speedFactor * dt;


	Vector3 target = m_Position + m_Forward;

	m_View = Matrix::CreateLookAt(m_Position, target, m_Up);
}

const Vector3& Camera::getPosition() const
{
	return m_Position;
}

void Camera::rotate(Vector3 axis, float rad)
{
	if (axis == Vector3(1.0f, 0.0f, 0.0f))
		m_Rotation.x = rad;
	if (axis == Vector3(0.0f, 1.0f, 0.0f))
		m_Rotation.y = rad;
	if (axis == Vector3(0.0f, 0.0f, 1.0f))
		m_Rotation.z = rad;
}

Matrix Camera::getView() const
{
	return m_View;
}

Matrix Camera::getProjection() const
{
	return m_Projection;
}

Matrix Camera::getRotation() const
{
	Matrix xRotMat = Matrix::CreateRotationX(m_Rotation.x);
	Matrix yRotMat = Matrix::CreateRotationY(m_Rotation.y);
	//Matrix zRotMat = Matrix::CreateRotationZ(rotation.z);
	Matrix finalRotMat = xRotMat * yRotMat;
	Vector3 forward = XMVector3TransformCoord(m_DefaultForward, finalRotMat);
	Vector3 right = XMVector3TransformCoord(m_DefaultRight, finalRotMat);
	Vector3 up = m_Forward.Cross(m_Right);
	up.Normalize();

	return Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 0.0f), forward, m_Up);
}

Matrix Camera::getViewProjection() const
{
	return m_View * m_Projection;
}
