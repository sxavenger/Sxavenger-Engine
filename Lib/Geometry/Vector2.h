#pragma once

template <typename T>
class Vector2 {
public:

	T x, y;

	
	//=========================================================================================
	// operator
	//=========================================================================================
	/* Add */
	Vector2 operator+(const Vector2& other) const {
		return { x + other.x, y + other.y };
	}
	void operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
	}

	/* Subtruct */
	Vector2 operator-(const Vector2& other) const {
		return { x - other.x, y - other.y };
	}
	void operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
	}

	/* Multiply */
	Vector2 operator*(const Vector2& other) const {
		return { x * other.x, y * other.y };
	}
	void operator*=(const Vector2& other) {
		x *= other.x;
		y *= other.y;
	}
	// scalar
	Vector2 operator*(T scalar) const {
		return { x * scalar, y * scalar };
	}
	void operator*=(T scalar) {
		x *= scalar;
		y += scalar;
	}

};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;