#pragma once
class Camera
{
public:
	Camera();
	~Camera();

	bool Init(float defaultSpeed, float fov, float aspect, Vector3 target, Vector3 position, float nearPlane, float farPlane);

	void update(float dt);

	const Vector3& getPosition() const;

	void rotate(Vector3 axis, float rad);

	Matrix getView() const;
	Matrix getProjection() const;
	Matrix getRotation() const;
	Matrix getViewProjection() const;

private:
	float m_DefaultSpeed;
	float m_Interia;

	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_RotationLag;

	Matrix m_View;
	Matrix m_Projection;

	Vector3 m_Forward;
	Vector3 m_Up;
	Vector3 m_Right;

	const Vector3 m_DefaultRight		= Vector3(1.0f, 0.0f, 0.0f);
	const Vector3 m_DefaultUp			= Vector3(0.0f, 1.0f, 0.0f);
	const Vector3 m_DefaultForward	= Vector3(0.0f, 0.0f, 1.0f);
};

