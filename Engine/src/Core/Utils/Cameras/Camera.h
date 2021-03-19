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
	float defaultSpeed;

	Vector3 position;
	Vector3 rotation;

	Matrix view;
	Matrix projection;

	Vector3 forward;
	Vector3 up;
	Vector3 right;

	const Vector3 defaultRight		= Vector3(1.0f, 0.0f, 0.0f);
	const Vector3 defaultUp			= Vector3(0.0f, 1.0f, 0.0f);
	const Vector3 defaultForward	= Vector3(0.0f, 0.0f, 1.0f);
};

