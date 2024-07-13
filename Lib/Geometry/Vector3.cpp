#include "Vector3.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f Normalize(const Vector3f& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	if (length == 0.0f) {
		return { 0.0f, 0.0f, 0.0f };
	}

	return { v.x / length, v.y / length, v.z / length };
}

float Length(const Vector3f& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float Dot(const Vector3f& x, const Vector3f& y) {
	return x.x * y.x + x.y * y.y + x.z * y.z;
}

Vector3f Cross(const Vector3f& x, const Vector3f& y) {
	return {
		x.y * y.z - x.z * y.y,
		x.z * y.x - x.x * y.z,
		x.x * y.y - x.y * y.x
	};
}

Vector3f Reflect(const Vector3f& i, const Vector3f& n) {
	float dot = Dot(i, n);
	Vector3f scalerNormal = n * (2.0f * dot);
	return i - scalerNormal;
}

Vector3f Lerp(const Vector3f& x, const Vector3f& y, float s) {
	return {
		x.x + s * (y.x - x.x),
		x.y + s * (y.y - x.y),
		x.z + s * (y.z - x.z)
	};
}