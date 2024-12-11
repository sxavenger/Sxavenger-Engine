#include "Collider.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerModule.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Collider class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Collider::Term() {
	SxavengerModule::EraseCollider(this);
}

void Collider::SetToCollection() {
	SxavengerModule::SetCollider(this);
}

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

void Collider::SetImGuiCommand() {
	ImGui::Checkbox("active", &isActive_);

	if (ImGui::TreeNode("transform component")) {
		TransformComponent::SetImGuiCommand();
		ImGui::TreePop();
	}

	if (!bounding_.has_value()) {
		return;
	}

	if (ImGui::TreeNode("boundings")) {

		// boundingの型の判定
		if (std::holds_alternative<CollisionBoundings::Sphere>(bounding_.value())) { //!< sphereの場合
			auto& sphere = std::get<CollisionBoundings::Sphere>(bounding_.value());

			ImGui::Text("Boundings: Sphere");
			ImGui::DragFloat("radius", &sphere.radius, 0.01f);

		} else if (std::holds_alternative<CollisionBoundings::Capsule>(bounding_.value())) {
			auto& capsule = std::get<CollisionBoundings::Capsule>(bounding_.value());

			ImGui::Text("Boundings: Capsule");

			if (ImGui::DragFloat3("direction", &capsule.direction.x, 0.01f)) {
				capsule.direction = Normalize(capsule.direction);
			}

			ImGui::DragFloat("radius", &capsule.radius, 0.01f);
			ImGui::DragFloat("length", &capsule.length, 0.01f);

		} else if (std::holds_alternative<CollisionBoundings::AABB>(bounding_.value())) { //!< AABBの場合
			auto& aabb = std::get<CollisionBoundings::AABB>(bounding_.value());

			ImGui::Text("Boundings: AABB");
			ImGui::DragFloat3("max", &aabb.max.x, 0.01f);
			ImGui::DragFloat3("min", &aabb.min.x, 0.01f);

			// minがmaxを上回らないようclamp
			aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
			aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);

			aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
			aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);

			aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
			aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);
		}

		ImGui::TreePop();
	}
}


