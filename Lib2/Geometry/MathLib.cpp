#include "MathLib.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MathLib methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector4f Lerp(const Vector4f& x, const Vector4f& y, float s) {
	return {
		x.x + s * (y.x - x.x),
		x.y + s * (y.y - x.y),
		x.z + s * (y.z - x.z),
		x.w + s * (y.w - x.w)
	};
}

bool IsNan(const Vector3f& v) {
	return Any(Flag3{std::isnan(v.x), std::isnan(v.y), std::isnan(v.z)});
}

bool IsNan(const Quaternion& q) {
	return Any(Flag4{ std::isnan(q.x), std::isnan(q.y), std::isnan(q.z), std::isnan(q.w)});
}

