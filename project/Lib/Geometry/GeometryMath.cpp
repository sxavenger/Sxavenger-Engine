#include "GeometryMath.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "VectorComparison.h"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float Length(const Vector2<float>& v) {
	return std::sqrt(v.x * v.x + v.y * v.y);
}

float Length(const Vector3<float>& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float Length(const Vector4<float>& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float Dot(const Vector2<float>& x, const Vector2<float>& y) {
	return x.x * y.x + x.y * y.y;
}

float Dot(const Vector3<float>& x, const Vector3<float>& y) {
	return x.x * y.x + x.y * y.y + x.z * y.z;
}

float Dot(const Vector4<float>& x, const Vector4<float>& y) {
	return x.x * y.x + x.y * y.y + x.z * y.z + x.w * y.w;
}

float Cross(const Vector2<float>& x, const Vector2<float>& y) {
	return x.x * y.y - x.y * y.x;
}

Vector3<float> Cross(const Vector3<float>& x, const Vector3<float>& y) {
	return {
		x.y * y.z - x.z * y.y,
		x.z * y.x - x.x * y.z,
		x.x * y.y - x.y * y.x
	};
}

Vector2<float> Normalize(const Vector2<float>& v) {
	return Length(v) != 0.0f ? v / Length(v) : Vector2<float>{};
}

Vector3<float> Normalize(const Vector3<float>& v) {
	return Length(v) != 0.0f ? v / Length(v) : Vector3<float>{};
}

Vector4<float> Normalize(const Vector4<float>& v) {
	return Length(v) != 0.0f ? v / Length(v) : Vector4<float>{};
}

Vector2<float> Saturate(const Vector2<float>& v) {
	return { std::clamp(v.x, 0.0f, 1.0f), std::clamp(v.y, 0.0f, 1.0f) };
}

Vector3<float> Saturate(const Vector3<float>& v) {
	return { std::clamp(v.x, 0.0f, 1.0f), std::clamp(v.y, 0.0f, 1.0f), std::clamp(v.z, 0.0f, 1.0f) };
}

Vector4<float> Saturate(const Vector4<float>& v) {
	return { std::clamp(v.x, 0.0f, 1.0f), std::clamp(v.y, 0.0f, 1.0f), std::clamp(v.z, 0.0f, 1.0f), std::clamp(v.w, 0.0f, 1.0f) };
}

Vector2<float> Abs(const Vector2<float>& v) {
	return { std::abs(v.x), std::abs(v.y) };
}

Vector3<float> Abs(const Vector3<float>& v) {
	return { std::abs(v.x), std::abs(v.y), std::abs(v.z) };
}

Vector4<float> Abs(const Vector4<float>& v) {
	return { std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w) };
}

Vector2<float> Lerp(const Vector2<float>& x, const Vector2<float>& y, float s) {
	return x + s * (y - x);
}

Vector3<float> Lerp(const Vector3<float>& x, const Vector3<float>& y, float s) {
	return x + s * (y - x);
}

Vector4<float> Lerp(const Vector4<float>& x, const Vector4<float>& y, float s) {
	return x + s * (y - x);
}

Vector2<float> Clamp(const Vector2<float>& v, const Vector2<float>& min, const Vector2<float>& max) {
	return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) };
}

Vector3<float> Clamp(const Vector3<float>& v, const Vector3<float>& min, const Vector3<float>& max) {
	return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z) };
}

Vector4<float> Clamp(const Vector4<float>& v, const Vector4<float>& min, const Vector4<float>& max) {
	return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z), std::clamp(v.w, min.w, max.w) };
}

Quaternion AxisAngle(const Vector3<float>& axis, float angle) {
	return Quaternion{ axis.x * std::sin(angle / 2.0f), axis.y * std::sin(angle / 2.0f), axis.z * std::sin(angle / 2.0f), std::cos(angle / 2.0f) };
}

Quaternion MakeAxisAngle(const Vector3<float>& axis, float angle) {
	return AxisAngle(axis, angle);
}

float Dot(const Quaternion& x, const Quaternion& y) {
	return x.x * y.x + x.y * y.y + x.z * y.z + x.w * y.w;
}

Quaternion ToQuaternion(const Vector3<float>& euler) {
	// 半角に変換
	float cy = std::cos(euler.y * 0.5f);
	float sy = std::sin(euler.y * 0.5f);
	float cp = std::cos(euler.x * 0.5f);
	float sp = std::sin(euler.x * 0.5f);
	float cr = std::cos(euler.z * 0.5f);
	float sr = std::sin(euler.z * 0.5f);

	Quaternion q = {};
	q.x = cr * sp * cy + sr * cp * sy; // x
	q.y = cr * cp * sy - sr * sp * cy; // y
	q.z = sr * cp * cy - cr * sp * sy; // z
	q.w = cr * cp * cy + sr * sp * sy; // w

	return q;
}

Quaternion FromToRotation(const Vector3<float>& from, const Vector3<float>& to) {
	Vector3f axis = Normalize(Cross(from, to));

	if (All(axis == kOrigin3<float>)) {
		if (from.x != 0.0f || from.y != 0.0f) {
			axis = { from.y, -from.x, 0.0f };

		} else {
			axis = { from.z, 0.0f, -from.x };
		}
	}

	return AxisAngle(axis, std::acos(Dot(from, to)));
}

Vector3<float> RotateVector(const Vector3<float>& v, const Quaternion& q) {
	Quaternion r = { v.x, v.y, v.z, 0.0f };
	Quaternion result = q * r * q.Conjugate();
	return { result.x, result.y, result.z };
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
	float dot = Dot(q0, q1);
	Quaternion q = q0;

	if (dot < 0.0f) {
		q = -q0;
		dot = -dot;
	}

	const float epsilon = 0.00005f;

	if (dot >= 1.0f - epsilon) {
		return (1.0f - t) * q + t * q1;
	}

	float theta = std::acos(dot);

	float scaler0 = std::sin(theta * (1.0f - t)) / std::sin(theta);
	float scaler1 = std::sin(theta * t) / std::sin(theta);

	return scaler0 * q + scaler1 * q1;
}

bool IsNan(const Vector2<float>& v) {
	return Any(Flag2{ std::isnan(v.x), std::isnan(v.y) });
}

bool IsNan(const Vector3<float>& v) {
	return Any(Flag3{ std::isnan(v.x), std::isnan(v.y), std::isnan(v.z) });
}

bool IsNan(const Vector4<float>& v) {
	return Any(Flag4{ std::isnan(v.x), std::isnan(v.y), std::isnan(v.z), std::isnan(v.w) });
}

bool IsNan(const Quaternion& q) {
	return Any(Flag4{ std::isnan(q.x), std::isnan(q.y), std::isnan(q.z), std::isnan(q.w) });
}
