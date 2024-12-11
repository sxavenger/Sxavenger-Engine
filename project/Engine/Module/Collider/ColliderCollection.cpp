#include "ColliderCollection.h"


//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "CollisionDetection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderCollection::CheckCollision() {
	SetupColliderState();
	CheckAllCollision();
	CallBack();
}

void ColliderCollection::Draw() {
}

void ColliderCollection::SystemDebugGui() {
}

void ColliderCollection::SetupColliderState() {
	for (auto& collider : colliders_) {
		collider->SetupColliderState();
	}
}

void ColliderCollection::CheckAllCollision() {
	// コンテナ内の当たり判定の総当たり
	auto itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// Aの次から回し, 重複をなくす
		auto itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void ColliderCollection::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	if (!(colliderA->IsActive() && colliderB->IsActive())) {
		return; //!< どちらか一方が非アクティブの場合
	}

	//!< 各コライダーが当たり判定を必要としているか確認
	bool isTargetB = colliderA->CheckCollisionTarget(colliderB);
	bool isTargetA = colliderB->CheckCollisionTarget(colliderA);

	// このペアの当たり判定が必要かの確認
	if (!(isTargetB || isTargetA)) {
		return; //!< どちらも当たり判定が必要ではない場合
	}

	const std::optional<CollisionBoundings::Boundings>& boundingA = colliderA->GetBoundings();
	const std::optional<CollisionBoundings::Boundings>& boundingB = colliderB->GetBoundings();
	Assert(boundingA.has_value() && boundingB.has_value(), "bounding is not set.");

	bool isCollision = CollisionDetection::CheckCollision(
		colliderA->GetPosition(), boundingA.value(),
		colliderB->GetPosition(), boundingB.value()
	);

	if (!isCollision) {
		return; //!< collider同士が当たっていない場合
	}

	if (isTargetA) {
		colliderB->OnCollision(colliderA);
	}

	if (isTargetB) {
		colliderA->OnCollision(colliderB);
	}
}

void ColliderCollection::CallBack() {
	for (auto& collider : colliders_) { //!< collider関数の呼び出し
		collider->CallbackOnCollision();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionDrawer::DrawCollider(const Collider* collider, const Color4f& color) {

	const std::optional<CollisionBoundings::Boundings>& bounding = collider->GetBoundings();

	if (!bounding.has_value()) {
		return;
	}

	Visitor visitor = {};
	visitor.position = collider->GetPosition();
	visitor.color    = color;

	std::visit(visitor, bounding.value());
}

void CollisionDrawer::DrawSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) {
	position, sphere, color;
}

void CollisionDrawer::DrawCapsule(const Vector3f& position, const CollisionBoundings::Capsule& capsule, const Color4f& color) {
	position, capsule, color;
}

void CollisionDrawer::DrawAABB(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color) {
	position, aabb, color;
}

void CollisionDrawer::DrawOBB(const Vector3f& position, const CollisionBoundings::OBB& obb, const Color4f& color) {
	position, obb, color;
}

