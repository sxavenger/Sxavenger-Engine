#include "Collider.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Collider class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Collider::SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere) {
	bounding_ = sphere;
}

void Collider::SetColliderBoundingCapsule(const CollisionBoundings::Capsule& capsule) {
	bounding_ = capsule;
}

void Collider::SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb) {
	bounding_ = aabb;
}

void Collider::SetColliderBoundingOBB(const CollisionBoundings::OBB& obb) {
	bounding_ = obb;
}

void Collider::SetupColliderState() {
	//!< 次フレームの準備
	for (auto& state : states_) {
		state.second[static_cast<uint8_t>(CollisionState::kPrev)] = state.second[static_cast<uint8_t>(CollisionState::kCurrent)];
		state.second.reset(static_cast<uint8_t>(CollisionState::kCurrent));
	}
}

void Collider::CallbackOnCollision() {
	for (auto it = states_.begin(); it != states_.end();) {

		if (it->second.test(static_cast<uint8_t>(CollisionState::kCurrent))
			&& !it->second.test(static_cast<uint8_t>(CollisionState::kPrev))) { //!< colliderが当たったかどうか

			if (collisionEnterFunction_) {
				collisionEnterFunction_(it->first);
			}

		} else if (!it->second.test(static_cast<uint8_t>(CollisionState::kCurrent))
			&& it->second.test(static_cast<uint8_t>(CollisionState::kPrev))) { //!< colliderが離れたかどうか

			if (collisionExitFunction_) {
				collisionExitFunction_(it->first);
			}

		} else if (it->second.none()) {
			it = states_.erase(it);
			continue;
		}

		++it;
	}
}

void Collider::OnCollision(Collider* other) {
	states_[other].set(static_cast<uint8_t>(CollisionState::kCurrent)); //!< 現在frameで当たった
}

void Collider::SetCollisionState(
	Collider* const collider,
	const std::optional<bool>& isHitCurrent, const std::optional<bool>& isHitPrev) {

	auto& state = states_[collider];

	if (isHitCurrent.has_value()) {
		state.set(static_cast<uint8_t>(CollisionState::kCurrent), isHitCurrent.value());
	}

	if (isHitPrev.has_value()) {
		state.set(static_cast<uint8_t>(CollisionState::kPrev), isHitPrev.value());
	}
}

const Collider::CollisionStatesBit Collider::GetState(Collider* other) const {
	if (states_.contains(other)) {
		return states_.at(other);
	}

	return {};
}

bool Collider::CheckCollisionTarget(const Collider* const other) const {
	return (targetId_ & other->typeId_).any();
}


