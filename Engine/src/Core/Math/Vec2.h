#pragma once

class Vec2
{
public:
	Vec2(float x, float y) : x(x), y(y) {};
	Vec2(float value) { x = y =  value; };


	// Vector operations
	void normalize()
	{
		float sum = this->x + this->y;
		this->x /= sum;
		this->y /= sum;
	}

	// Operators
	Vec2 operator +(const Vec2& other)
	{
		return Vec2(this->x + other.x, this->y + other.y);
	}

	Vec2 operator -(const Vec2& other)
	{
		return Vec2(this->x - other.x, this->y - other.y);
	}

	Vec2 operator *(const Vec2& other)
	{
		return Vec2(this->x * other.x, this->y * other.y);
	}

	Vec2 operator /(const Vec2& other)
	{
		return Vec2(this->x / other.x, this->y / other.y);
	}

	Vec2& operator +=(const Vec2& other)
	{
		this->x += other.x;
		this->y += other.y;

		return *this;
	}

	Vec2& operator -=(const Vec2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Vec2& operator *=(const Vec2& other)
	{
		this->x *= other.x;
		this->y *= other.y;

		return *this;
	}

	Vec2& operator /=(const Vec2& other)
	{
		this->x /= other.x;
		this->y /= other.y;

		return *this;
	}

	float x, y;
};
