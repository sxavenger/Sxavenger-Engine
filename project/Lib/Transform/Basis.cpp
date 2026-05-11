#include "Basis.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* directx
#include <DirectXMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Basis class methods
////////////////////////////////////////////////////////////////////////////////////////////

Basis Basis::MakeScale(float v) noexcept {
	return {
		v, 0.0f, 0.0f,
		0.0f, v, 0.0f,
		0.0f, 0.0f, v
	};
}

Basis Basis::MakeScale(const Vector3f& v) noexcept {
	return {
		v.x, 0.0f, 0.0f,
		0.0f, v.y, 0.0f,
		0.0f, 0.0f, v.z
	};
}

Basis Basis::MakeRotateX(float v) noexcept {
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, std::cos(v), std::sin(v),
		0.0f, -std::sin(v), std::cos(v)
	};
}

Basis Basis::MakeRotateY(float v) noexcept {
	return {
		std::cos(v), 0.0f, -std::sin(v),
		0.0f, 1.0f, 0.0f,
		std::sin(v), 0.0f, std::cos(v)
	};
}

Basis Basis::MakeRotateZ(float v) noexcept {
	return {
		std::cos(v), std::sin(v), 0.0f,
		-std::sin(v), std::cos(v), 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Basis Basis::MakeRotateEuler(const Vector3f& euler) noexcept {
	return MakeRotateX(euler.x) * MakeRotateY(euler.y) * MakeRotateZ(euler.z);
}

Basis Basis::MakeRotateQuaternion(const Quaternion& q) noexcept {
	return {
		q.w * q.w + q.imaginary.x * q.imaginary.x - q.imaginary.y * q.imaginary.y - q.imaginary.z * q.imaginary.z, 2.0f * (q.imaginary.x * q.imaginary.y + q.w * q.imaginary.z), 2.0f * (q.imaginary.x * q.imaginary.z - q.w * q.imaginary.y),
		2.0f * (q.imaginary.x * q.imaginary.y - q.w * q.imaginary.z), q.w * q.w - q.imaginary.x * q.imaginary.x + q.imaginary.y * q.imaginary.y - q.imaginary.z * q.imaginary.z, 2.0f * (q.imaginary.y * q.imaginary.z + q.w * q.imaginary.x),
		2.0f * (q.imaginary.x * q.imaginary.z + q.w * q.imaginary.y), 2.0f * (q.imaginary.y * q.imaginary.z - q.w * q.imaginary.x), q.w * q.w - q.imaginary.x * q.imaginary.x - q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z,
	};
}


