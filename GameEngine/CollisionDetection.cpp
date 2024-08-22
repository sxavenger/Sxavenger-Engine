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

	switch (boundingA.index()) {
		case CollisionBoundings::BoundingsType::kSphere:
			{
				//!< sphereのboundingの取得
				const auto& sphereA = std::get<CollisionBoundings::Sphere>(boundingA);

				switch (boundingB.index()) {
					case CollisionBoundings::BoundingsType::kSphere:
						{
							//!< sphereのboundingの取得
							const auto& sphereB = std::get<CollisionBoundings::Sphere>(boundingB);

							return SphereTo(posisionA, sphereA, posisionB, sphereB);
						}
						break;

					case CollisionBoundings::BoundingsType::kAABB:
						{
							//!< aabbのboundingの取得
							const auto& aabbB = std::get<CollisionBoundings::AABB>(boundingB);

							return SphereToAABB(posisionA, sphereA, posisionB, aabbB);
						}
						break;

					case CollisionBoundings::BoundingsType::kOBB:
						return false;
						break;

					default:
						assert(false);
						break;
				}

			}
			break;

		case CollisionBoundings::BoundingsType::kAABB:
			{
				const auto& aabbA = std::get<CollisionBoundings::AABB>(boundingA);

				switch (boundingB.index()) {
					case CollisionBoundings::BoundingsType::kSphere:
						{
							//!< sphereのboundingの取得
							const auto& sphereB = std::get<CollisionBoundings::Sphere>(boundingB);

							return SphereToAABB(posisionB, sphereB, posisionA, aabbA);
						}
						break;

					case CollisionBoundings::BoundingsType::kAABB:
						{
							//!< aabbのboundingの取得
							const auto& aabbB = std::get<CollisionBoundings::AABB>(boundingB);

							return AABBTo(posisionA, aabbA, posisionB, aabbB);
						}
						break;

					case CollisionBoundings::BoundingsType::kOBB:
						return false;
						break;

					default:
						assert(false);
						break;
				}
			}
			break;
		
		case CollisionBoundings::BoundingsType::kOBB:
			return false; //!< todo: OBBの当たり判定の追加
			break;

		default:
			assert(false); //!< indexの例外
			break;
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

bool CollisionDetection::SphereToAABB(
	const Vector3f& posisionA, const CollisionBoundings::Sphere& sphereA,
	const Vector3f& posisionB, const CollisionBoundings::AABB& aabbB) {

	Vector3f closestPoint = Clamp(posisionA, aabbB.localMin + posisionB, aabbB.localMax + posisionB);

	float distance = Length(closestPoint - posisionA);

	if (distance <= sphereA.radius) {
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
