#include "Quaternion.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// anonymose namespacae
////////////////////////////////////////////////////////////////////////////////////////////

namespace {

	//! @brief vector結果を出力するための構造体
	struct float3 {
		float x, y, z;
	};

	//! @brief QuartanionのVector部分のdot演算用関数
	float DotVector(const Quaternion& q, const Quaternion& r) {
		return q.x * r.x + q.y * r.y + q.z * r.z;
	}

	//! @brief QuartanionのVector部分のcross演算用関数
	float3 CrossVector(const Quaternion& q, const Quaternion& r) {
		return {
			q.y * r.z - q.z * r.y,
			q.z * r.x - q.x * r.z,
			q.x * r.y - q.y * r.x
		};
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion methods
////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::Identity() {
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Quaternion::Conjugation() const {
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
	Quaternion conj = Conjugation();
	float norm2 = x * x + y * y + z * z + w * w;

	return { conj.x / norm2, conj.y / norm2, conj.z / norm2, conj.w / norm2 };
}

Quaternion& Quaternion::operator*=(const Quaternion& q) {

	Quaternion result;

	float3 cross = CrossVector(*this, q);
	float dot = DotVector(*this, q);

	result.x = cross.x + q.w * x + w * q.x;
	result.y = cross.y + q.w * y + w * q.y;
	result.z = cross.z + q.w * z + w * q.z;
	result.w = w * q.w - dot;

	*this = result;
	return *this;
}

Quaternion operator+(const Quaternion& q, const Quaternion& r) {
	return { q.x + r.x , q.y + r.y, q.z + r.z, q.w + r.w };
}

Quaternion operator*(const Quaternion& q, const Quaternion& r) {

	Quaternion result;

	float3 cross = CrossVector(q, r);
	float dot = DotVector(q, r);

	result.x = cross.x + r.w * q.x + q.w * r.x;
	result.y = cross.y + r.w * q.y + q.w * r.y;
	result.z = cross.z + r.w * q.z + q.w * r.z;
	result.w = q.w * r.w - dot;

	return result;
}

Quaternion operator*(const Quaternion& q, float scaler) {
	return { q.x * scaler, q.y * scaler, q.z * scaler, q.w * scaler };
}

Quaternion operator*(float scaler, const Quaternion& q) {
	return { q.x * scaler, q.y * scaler, q.z * scaler, q.w * scaler };
}

float Dot(const Quaternion& q, const Quaternion& r) {
	return q.x * r.x + q.y * r.y + q.z * r.z + q.w * r.w;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3f& axis, float angle) {

	Quaternion result;

	result.x = axis.x * std::sin(angle / 2.0f);
	result.y = axis.y * std::sin(angle / 2.0f);
	result.z = axis.z * std::sin(angle / 2.0f);
	result.w = std::cos(angle / 2.0f);

	return result;
}

Vector3f RotateVector(const Vector3f& v, const Quaternion& q) {
	Quaternion r = { v.x, v.y, v.z, 0.0f };
	Quaternion result = q * r * q.Conjugation();

	return { result.x, result.y, result.z };
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {

	float dot = Dot(q0, q1);
	Quaternion q = q0;

	if (dot < 0.0f) {
		q = -q0;
		dot = -dot;
	}

	float theta = std::acos(dot);

	float scaler0 = std::sin(theta * (1.0f - t)) / std::sin(theta);
	float scaler1 = std::sin(theta * t) / std::sin(theta);

	return scaler0 * q + scaler1 * q1;

}
