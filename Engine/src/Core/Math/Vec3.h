#pragma once

class Vec3
{
public:
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {};
	Vec3(float value) { x = y = z = value; };

	// Vector operations
	void normalize()
	{
		float sum = this->x + this->y + this->z;
		this->x /= sum;
		this->y /= sum;
		this->z /= sum;
	}


	// Operators
	Vec3 operator +(const Vec3& other)
	{
		return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
	}

	Vec3 operator -(const Vec3& other)
	{
		return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
	}

	Vec3 operator *(const Vec3& other)
	{
		return Vec3(this->x * other.x, this->y * other.y, this->z * other.z);
	}

	Vec3 operator /(const Vec3& other)
	{
		return Vec3(this->x / other.x, this->y / other.y, this->z / other.z);
	}

	Vec3& operator +=(const Vec3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;

		return *this;
	}

	Vec3& operator -=(const Vec3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;

		return *this;
	}

	Vec3& operator *=(const Vec3& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;

		return *this;
	}

	Vec3& operator /=(const Vec3& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;

		return *this;
	}

	float x, y, z;
};

