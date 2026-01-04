#include "CollisionHelper.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionHelper class methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CollisionHelper::PushBackAABB(ColliderComponent* target, ColliderComponent* source) {
	if (!std::holds_alternative<CollisionBoundings::AABB>(target->GetBoundings())) {
		return {}; // target is not AABB
	}

	if (!std::holds_alternative<CollisionBoundings::AABB>(source->GetBoundings())) {
		return {}; // source is not AABB
	}

	const auto& targetAABB     = std::get<CollisionBoundings::AABB>(target->GetBoundings());
	auto targetTransform       = target->RequireTransform();
	const auto& targetPosition = targetTransform->GetPosition();

	auto& sourceAABB           = std::get<CollisionBoundings::AABB>(source->GetBoundings());
	auto sourceTransform       = source->RequireTransform();
	const auto& sourcePosition = sourceTransform->GetPosition();

	Vector3f pushback = {}; //!< 押し戻し量

	Vector3f overlap
		= Vector3f::Min(targetAABB.max + targetPosition, sourceAABB.max + sourcePosition)
		- Vector3f::Max(targetAABB.min + targetPosition, sourceAABB.min + sourcePosition);

	if (overlap.x < overlap.y && overlap.x < overlap.z) {

		//!< x軸方向の押し戻し
		pushback.x = targetAABB.max.x + targetPosition.x > sourceAABB.max.x + sourcePosition.x
			? overlap.x   //!< 右面
			: -overlap.x; //!< 左面

	} else if (overlap.y < overlap.x && overlap.y < overlap.z) {

		//!< y軸方向の押し戻し
		pushback.y = targetAABB.max.y + targetPosition.y > sourceAABB.max.y + sourcePosition.y
			? overlap.y   //!< 下面
			: -overlap.y; //!< 上面

	} else {

		//!< z軸方向の押し戻し
		pushback.z = targetAABB.max.z + targetPosition.z > sourceAABB.max.z + sourcePosition.z
			? overlap.z   //!< 裏面
			: -overlap.z; //!< 前面

	}

	//!< 押し戻し量をtargetの位置に加算
	targetTransform->translate += pushback;

	return pushback.Normalize();
}
