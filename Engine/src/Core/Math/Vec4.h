#pragma once

class Vec4
{
public:
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	Vec4(float value) { x = y = z = w = value;  };

	// Vector operations
	void normalize()
	{
		float sum = this->x + this->y + this->z + this->w;
		this->x /= sum;
		this->y /= sum;
		this->z /= sum;
		this->w /= sum;
	}

	// Operators
	Vec4 operator +(const Vec4& other)
	{
		return Vec4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
	}

	Vec4 operator -(const Vec4& other)
	{
		return Vec4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
	}

	Vec4 operator *(const Vec4& other)
	{
		return Vec4(this->x * other.x, this->y * other.y, this->z * other.z, this->w * other.w);
	}

	Vec4 operator /(const Vec4& other)
	{
		return Vec4(this->x / other.x, this->y / other.y, this->z / other.z, this->w / other.w);
	}

	Vec4& operator +=(const Vec4& other) 
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;

		return *this;
	}
	
	Vec4& operator -=(const Vec4& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;

		return *this;
	}

	Vec4& operator *=(const Vec4& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		this->w *= other.w;

		return *this;
	}

	Vec4& operator /=(const Vec4 & other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
		this->w /= other.w;

		return *this;
	}

	float x, y, z, w;
};

