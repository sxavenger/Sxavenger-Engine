#include "Collider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Collider namespace
////////////////////////////////////////////////////////////////////////////////////////////

bool Collider::AABBToAABB(const AABB& aabb, const AABB& b) {

	if ((aabb.min.x <= b.max.x && aabb.max.x >= b.min.x) &&
		(aabb.min.y <= b.max.y && aabb.max.y >= b.min.y) &&
		(aabb.min.z <= b.max.z && aabb.max.z >= b.min.z)) {

		return true;
	}

	return false;
}

bool Collider::AABBToPoint(const AABB& aabb, const Vector3f& point) {

	if ((aabb.min.x <= point.x && aabb.max.x >= point.x) &&
		(aabb.min.y <= point.y && aabb.max.y >= point.y) &&
		(aabb.min.z <= point.z && aabb.max.z >= point.z)) {

		return true;
	}

	return false;
}
