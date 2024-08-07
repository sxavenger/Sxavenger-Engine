#include "ColliderManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderManager::Init() {
}

void ColliderManager::Term() {
}

void ColliderManager::Update() {
	CheckAllCollision(); //!< 全当たり判定の計算

	for (auto& collider : colliders_) { //!< collider関数の呼び出し
		collider->CallOnCollisionMethods();
	}
}

ColliderManager* ColliderManager::GetInstance() {
	static ColliderManager instance;
	return &instance;
}

void ColliderManager::CheckAllCollision() {
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

void ColliderManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	bool isCollision = CollisionDetection::CheckCollision(
		colliderA->GetColliderPosition(), colliderA->GetBounding(),
		colliderB->GetColliderPosition(), colliderB->GetBounding()
	);

	if (isCollision) { //!< 衝突してた場合
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	}
}
