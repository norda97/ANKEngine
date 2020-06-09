#pragma once
class Camera
{
public:
	Camera();
	~Camera();

	bool init(float fov, float aspect, Vector3 target, Vector3 position, float nearPlane, float farPlane);

	Matrix getView() const;
	Matrix getProjection() const;
	Matrix getViewProjection() const;

private:
	Vector3 position;
	Vector3 rotation;

	Matrix view;
	Matrix projection;

	Vector3 forward;
	Vector3 right;
};

