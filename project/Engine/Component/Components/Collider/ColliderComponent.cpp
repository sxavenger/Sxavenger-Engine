#include "ColliderComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderComponent::SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere) {
	bounding_ = sphere;
}

void ColliderComponent::SetColliderBoundingCapsule(const CollisionBoundings::Capsule& capsule) {
	bounding_ = capsule;
}

void ColliderComponent::SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb) {
	bounding_ = aabb;
}

void ColliderComponent::SetColliderBoundingOBB(const CollisionBoundings::OBB& obb) {
	bounding_ = obb;
}

void ColliderComponent::ResetColliderBounding() {
	bounding_ = std::nullopt;
}

void ColliderComponent::UpdateColliderState() {
	//!< 次フレームの準備
	for (auto& [ptr, state] : states_) {
		state <<= 1; //!< current情報をprevにシフト
	}
}

void ColliderComponent::CallbackOnCollision() {
	for (auto it = states_.begin(); it != states_.end();) {
		switch (it->second.to_ulong()) {
			case 0b00: //!< prev 0, current 0
				it = states_.erase(it);
				continue;

			case 0b01: //!< prev 0, current 1
				if (collisionEnterFunction_) {
					collisionEnterFunction_(it->first);
				}
				break;

			case 0b10: //!< prev 1, current 0
				if (collisionExitFunction_) {
					collisionExitFunction_(it->first);
				}
				break;

			case 0b11: //!< prev 1, current 1
				break;
		};

		++it;
	}
}

void ColliderComponent::OnCollision(ColliderComponent* other) {
	states_[other].set(static_cast<bool>(CollisionState::kCurrent)); //!< 現在frameで当たった
}

void ColliderComponent::SetCollisionState(
	ColliderComponent* const other,
	const std::optional<bool>& isHitCurrent, const std::optional<bool>& isHitPrev) {

	auto& state = states_[other];

	if (isHitCurrent.has_value()) {
		state.set(static_cast<bool>(CollisionState::kCurrent), isHitCurrent.value());
	}

	if (isHitPrev.has_value()) {
		state.set(static_cast<bool>(CollisionState::kPrev), isHitPrev.value());
	}

}

TransformComponent* ColliderComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
