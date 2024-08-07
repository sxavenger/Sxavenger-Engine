#include "CollisionDetection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDetection class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool CollisionDetection::CheckCollision(
	const Vector3f& posisionA, const CollisionBoundings::Boundings& boundingA,
	const Vector3f& posisionB, const CollisionBoundings::Boundings& boundingB) {

	posisionA, posisionB;
	boundingA, boundingB;

	// テストでsphere to sphereだけ判定をとる
	if (std::holds_alternative<CollisionBoundings::Sphere>(boundingA) && std::holds_alternative<CollisionBoundings::Sphere>(boundingB)) {
		//!< boundingがsphere同士なら
		
		const auto& sphereA = std::get<CollisionBoundings::Sphere>(boundingA);
		const auto& sphereB = std::get<CollisionBoundings::Sphere>(boundingB);

		return SphereTo(posisionA, sphereA, posisionB, sphereB);
	}

	if (std::holds_alternative<CollisionBoundings::AABB>(boundingA) && std::holds_alternative<CollisionBoundings::AABB>(boundingB)) {
		//!< boundingがAABB同士なら
		
		const auto& aabbA = std::get<CollisionBoundings::AABB>(boundingA);
		const auto& aabbB = std::get<CollisionBoundings::AABB>(boundingB);

		return AABBTo(posisionA, aabbA, posisionB, aabbB);
	}

	return false; //!< todo: 当たり判定の追加
}

bool CollisionDetection::SphereTo(
	const Vector3f& positionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& positionB, const CollisionBoundings::Sphere& sphereB) {

	float distance = Length(positionA - positionB);

	if (distance <= sphereA.radius + sphereB.radius) {
		return true;
	}

	return false;
}

bool CollisionDetection::AABBTo(
	const Vector3f& posisionA, const CollisionBoundings::AABB& aabbA,
	const Vector3f& posisionB, const CollisionBoundings::AABB& aabbB) {

	if (All(aabbA.localMin + posisionA <= aabbB.localMax + posisionB) 
		&& All(aabbA.localMax + posisionA >= aabbB.localMin + posisionB)) {

		return true;
	}

	return false;
}
