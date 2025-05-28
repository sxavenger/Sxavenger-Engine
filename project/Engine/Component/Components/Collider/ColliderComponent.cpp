#include "ColliderComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* collider
#include "CollisionCallbackCollection.h"

//* engine
#include <Engine/System/UI/SxImGui.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderComponent::ShowComponentInspector() {
	ImGui::Text("boundings");
	ImGui::Separator();

	if (!bounding_.has_value()) {
		ImGui::TextDisabled("bounding not set...");
		return;
	}

	auto& bounding = bounding_.value();

	// boundingの型の判定
	if (std::holds_alternative<CollisionBoundings::Sphere>(bounding)) { //!< sphereの場合
		auto& sphere = std::get<CollisionBoundings::Sphere>(bounding);

		ImGui::Text("Boundings: Sphere");
		SxImGui::DragFloat("radius", &sphere.radius, 0.01f, 0.0f);

	} else if (std::holds_alternative<CollisionBoundings::Capsule>(bounding)) {
		auto& capsule = std::get<CollisionBoundings::Capsule>(bounding);

		ImGui::Text("Boundings: Capsule");

		if (SxImGui::DragVector3("direction", &capsule.direction.x, 0.01f)) {
			capsule.direction = capsule.direction.Normalize();
		}

		SxImGui::DragFloat("radius", &capsule.radius, 0.01f, 0.0f);
		SxImGui::DragFloat("length", &capsule.length, 0.01f, 0.0f);

	} else if (std::holds_alternative<CollisionBoundings::AABB>(bounding)) { //!< AABBの場合
		auto& aabb = std::get<CollisionBoundings::AABB>(bounding);

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
}

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

void ColliderComponent::CallbackOnCollision(const CollisionCallbackCollection* collection) {
	for (auto it = states_.begin(); it != states_.end();) {
		switch (it->second.to_ulong()) {
			case 0b00: //!< prev 0, current 0
				it = states_.erase(it);
				continue;

			case 0b01: //!< prev 0, current 1
				collection->CallbackOnCollisionEnter(this, it->first);
				break;

			case 0b10: //!< prev 1, current 0
				collection->CallbackOnCollisionExit(this, it->first);
				break;

			case 0b11: //!< prev 1, current 1
				collection->CallbackOnCollisionStay(this, it->first);
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
