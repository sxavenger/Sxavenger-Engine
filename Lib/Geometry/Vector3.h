#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class Vector3 {
public:

	T x, y, z;


	//=========================================================================================
	// operator
	//=========================================================================================

	/* Add */
	Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}
	void operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	/* Subtract */
	Vector3 operator-(const Vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}
	void operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	/* Multiply */
	Vector3 operator*(const Vector3& other) const {
		return { x * other.x, y * other.y, z * other.z };
	}
	void operator*=(const Vector3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}
	// scalar
	Vector3 operator*(T scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}
	void operator*=(T scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using Vector3f = Vector3<float>;

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////
constexpr const Vector3f origin     = { 0.0f, 0.0f, 0.0f };
constexpr const Vector3f unitVector = { 1.0f, 1.0f, 1.0f };

////////////////////////////////////////////////////////////////////////////////////////////
// Vector namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Vector {

	Vector3f Normalize(const Vector3f& v);

	float Length(const Vector3f& v);

	float Dot(const Vector3f& x, const Vector3f& y);

	Vector3f Cross(const Vector3f& x, const Vector3f& y);

	Vector3f Reflect(const Vector3f& i, const Vector3f& n);

	Vector3f Lerp(const Vector3f& x, const Vector3f& y, float s);

}