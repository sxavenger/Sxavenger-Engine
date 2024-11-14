#include "Collider.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

// imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Collider base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Collider::Init() {
	SxavengerGame::SetCollider(this);
}

void Collider::Term() {
	SxavengerGame::EraseCollider(this);
}

void Collider::SetColliderBoundingSphere(const CollisionBoundings::Sphere& sphere) {
	bounding_ = sphere;
}

void Collider::SetColliderBoundingAABB(const CollisionBoundings::AABB& aabb) {
	bounding_ = aabb;
}

void Collider::CallbackOnCollision() {

	for (auto it = states_.begin(); it != states_.end();) {

		if (it->second.isHit && !it->second.isPreHit) { //!< colliderが当たったかどうか
			if (onCollisionEnterFunc_) {
				onCollisionEnterFunc_(it->first);
			}

		} else if (!it->second.isHit && it->second.isPreHit) { //!< colliderが離れたかどうか
			if (onCollisionExitFunc_) {
				onCollisionExitFunc_(it->first);
			}
		}

		//!< 次フレームの準備
		it->second.isPreHit = it->second.isHit;
		it->second.isHit    = false;

		// 当たらなくなったので削除
		if (!it->second.isHit && !it->second.isPreHit) {
			it = states_.erase(it);
			continue;
		}

		++it;
	}
}

void Collider::OnCollision(Collider* other) {
	states_[other].isHit = true; //!< 現在frameで当たった
}

const Vector3f& Collider::GetColliderPosition() const {
	return position_;
}

bool Collider::ShouldCheckForCollision(const Collider* const other) const {
	return other->typeId_ & this->targetTypeId_;
}

void Collider::SetCollisionState(
	Collider* const collider,
	const std::optional<bool>& isHit, const std::optional<bool>& isPreHit) {

	// 対象があるか確認
	auto it = states_.find(collider);
	if (it == states_.end()) { //!< 対象が見つからなかった場合, return
		return;
	}

	if (isHit) { //!< 変更要求がされていた場合
		it->second.isHit = isHit.value();
	}

	if (isPreHit) { //!< 変更要求がされていた場合
		it->second.isPreHit = isPreHit.value();
	}
}

void Collider::SetColliderImGuiCommand() {

	// boundingの型の判定
	if (std::holds_alternative<CollisionBoundings::Sphere>(bounding_)) { //!< sphereの場合
		auto& sphere = std::get<CollisionBoundings::Sphere>(bounding_);

		ImGui::Text("Boundings: Sphere");
		ImGui::DragFloat("radius", &sphere.radius, 0.01f);

	} else if (std::holds_alternative<CollisionBoundings::AABB>(bounding_)) { //!< AABBの場合
		auto& aabb = std::get<CollisionBoundings::AABB>(bounding_);

		ImGui::Text("Boundings: AABB");
		ImGui::DragFloat3("max", &aabb.localMax.x, 0.01f);
		ImGui::DragFloat3("min", &aabb.localMin.x, 0.01f);

		// minがmaxを上回らないようclamp
		aabb.localMin.x = (std::min)(aabb.localMin.x, aabb.localMax.x);
		aabb.localMax.x = (std::max)(aabb.localMin.x, aabb.localMax.x);

		aabb.localMin.y = (std::min)(aabb.localMin.y, aabb.localMax.y);
		aabb.localMax.y = (std::max)(aabb.localMin.y, aabb.localMax.y);

		aabb.localMin.z = (std::min)(aabb.localMin.z, aabb.localMax.z);
		aabb.localMax.z = (std::max)(aabb.localMin.z, aabb.localMax.z);
	}

}
