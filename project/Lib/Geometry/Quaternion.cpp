#include "Quaternion.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "GeometryMath.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion class methods
////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::Conjugate() const noexcept {
	return { -imaginary, w };
}

float Quaternion::Norm() const noexcept {
	return std::sqrt(imaginary.x * imaginary.x + imaginary.y * imaginary.y + imaginary.z * imaginary.z + w * w);
}

Quaternion Quaternion::Identity() noexcept {
	return { 0, 0, 0, 1 };
}

Quaternion Quaternion::AxisAngle(const Vector3f& axis, float angle) noexcept {
	return { axis * std::sin(angle * 0.5f), std::cos(angle * 0.5f) };
}

Vector3f Quaternion::RotateVector(const Vector3f& v, const Quaternion& q) noexcept {
	Quaternion r = { v.x, v.y, v.z, 0.0f };
	Quaternion result = q * r * q.Conjugate();
	return result.imaginary;
}

Quaternion Quaternion::ToQuaternion(const Vector3f& euler) noexcept {
	// 半角に変換
	float cy = std::cos(euler.y * 0.5f);
	float sy = std::sin(euler.y * 0.5f);
	float cp = std::cos(euler.x * 0.5f);
	float sp = std::sin(euler.x * 0.5f);
	float cr = std::cos(euler.z * 0.5f);
	float sr = std::sin(euler.z * 0.5f);

	Quaternion q = {
		cr * sp * cy + sr * cp * sy,
		cr * cp * sy - sr * sp * cy,
		sr * cp * cy - cr * sp * sy,
		cr * cp * cy + sr * sp * sy,
	};

	return q;
}

float Quaternion::Dot(const Quaternion& x, const Quaternion& y) noexcept {
	return Vector3f::Dot(x.imaginary, y.imaginary) + x.w * y.w;
}

Quaternion Quaternion::Slerp(const Quaternion& x, const Quaternion& y, float t) noexcept {
	float dot = Quaternion::Dot(x, y);

	Quaternion q = x;

	if (dot < 0.0f) {
		q   = -x;
		dot = -dot;
	}

	if (dot >= 1.0f - kEpsilon) {
		return (1.0f - t) * q + t * y;
	}

	float theta = std::acos(dot);

	float scaler0 = std::sin(theta * (1.0f - t)) / std::sin(theta);
	float scaler1 = std::sin(theta * t) / std::sin(theta);

	return scaler0 * q + scaler1 * y;
}

Quaternion Quaternion::FromToRotation(const Vector3f& from, const Vector3f& to) noexcept {
	float dot = Vector3f::Dot(from, to);

	// fromとtoが同じ方向の場合
	if (dot >= 1.0f - kEpsilon) {
		return Quaternion::Identity();
	}

	// fromとtoが逆方向の場合
	if (dot <= -1.0f + kEpsilon) {
		const Vector3f orthogonal = std::abs(from.x) > 1.0f - kEpsilon ? kUnitY3<float> : kUnitX3<float>;
		return { Vector3f::Cross(from, orthogonal).Normalize(), 0.0f };
	}

	Vector3f axis = Vector3f::Cross(from, to).Normalize();
	float angle   = std::acos(dot);

	return Quaternion::AxisAngle(axis, angle);
}

Quaternion Quaternion::LookForward(const Vector3f& forward, const Vector3f& up) noexcept {
	Quaternion q = Quaternion::FromToRotation(kForward3<float>, forward);
	Vector3f right = Vector3f::Cross(up, forward).Normalize();
	Vector3f newUp = Vector3f::Cross(forward, right);

	Vector3f modifed = Quaternion::RotateVector(kUp3<float>, q);
	return Quaternion::FromToRotation(modifed, newUp) * q;
}

Quaternion Quaternion::Normalize() const noexcept {
	float norm = Norm();
	return { imaginary / norm, w / norm };
}

Quaternion Quaternion::Inverse() const noexcept {
	Quaternion conj = Conjugate();
	float norm2 = imaginary.x * imaginary.x + imaginary.y * imaginary.y + imaginary.z * imaginary.z + w * w;

	return { conj.imaginary / norm2, conj.w / norm2 };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) noexcept {
	return { lhs.imaginary + rhs.imaginary, lhs.w + rhs.w };
}

Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) noexcept {
	return { lhs.imaginary - rhs.imaginary, lhs.w - rhs.w };
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) noexcept {
	Vector3f cross = Vector3f::Cross(lhs.imaginary, rhs.imaginary);
	float dot      = Vector3f::Dot(lhs.imaginary, rhs.imaginary);
	return { cross + rhs.imaginary * lhs.w + lhs.imaginary * rhs.w, lhs.w * rhs.w - dot };
}

Quaternion operator*(const Quaternion& lhs, float rhs) noexcept {
	return { lhs.imaginary * rhs, lhs.w * rhs };
}

Quaternion operator*(float lhs, const Quaternion& rhs) noexcept {
	return { rhs.imaginary * lhs, rhs.w * lhs };
}
