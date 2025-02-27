#include "Quaternion.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "GeometryMath.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion methods
////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::Identity() {
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Quaternion::Conjugate() const {
	return { -x, -y, -z, w };
}

float Quaternion::Norm() const {
	return std::sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::Normalize() const {
	float norm = Norm();

	return { x / norm, y / norm, z / norm, w / norm };
}

Quaternion Quaternion::Inverse() const {
	Quaternion conj = Conjugate();
	float norm2 = x * x + y * y + z * z + w * w;

	return { conj.x / norm2, conj.y / norm2, conj.z / norm2, conj.w / norm2 };
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs) {

	Quaternion result = {};

	Vector3<float> cross = Cross({ x, y, z }, { rhs.x, rhs.y, rhs.z });
	float dot            = Dot({ x, y, z }, { rhs.x, rhs.y, rhs.z });

	result.x = cross.x + rhs.w * x + w * rhs.x;
	result.y = cross.y + rhs.w * y + w * rhs.y;
	result.z = cross.z + rhs.w * z + w * rhs.z;
	result.w = w * rhs.w - dot;

	*this = result;
	return *this;
}

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
	return { lhs.x + rhs.x , lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {

	Quaternion result = {};

	Vector3<float> cross = Cross({ lhs.x, lhs.y, lhs.z }, { rhs.x, rhs.y, rhs.z });
	float dot              = Dot({ lhs.x, lhs.y, lhs.z }, { rhs.x, rhs.y, rhs.z });

	result.x = cross.x + rhs.w * lhs.x + lhs.w * rhs.x;
	result.y = cross.y + rhs.w * lhs.y + lhs.w * rhs.y;
	result.z = cross.z + rhs.w * lhs.z + lhs.w * rhs.z;
	result.w = lhs.w * rhs.w - dot;

	return result;
}

Quaternion operator*(const Quaternion& lhs, float rhs) {
	return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}

Quaternion operator*(float lhs, const Quaternion& rhs) {
	return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}
