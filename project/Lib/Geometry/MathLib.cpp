#include "MathLib.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MathLib methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IsNan(const Vector3f& v) {
	return Any(Flag3{std::isnan(v.x), std::isnan(v.y), std::isnan(v.z)});
}

bool IsNan(const Quaternion& q) {
	return Any(Flag4{ std::isnan(q.x), std::isnan(q.y), std::isnan(q.z), std::isnan(q.w)});
}

Vector3f Project(const Vector3f& v1, const Vector3f& v2) {
	Vector3f normalv2 = Normalize(v2);
	Vector3f result = normalv2 * Dot(v1, normalv2);

	return result;
}

