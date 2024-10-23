#include "ColliderManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerGame.h>

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

void ColliderManager::DrawColliders() {
	for (const auto& collider : colliders_) {
		DrawCollider(collider);
	}
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

	//!< 各コライダーが当たり判定を必要としているか確認
	bool isATargetingB = colliderA->ShouldCheckForCollision(colliderB);
	bool isBTargetingA = colliderB->ShouldCheckForCollision(colliderA);

	// このペアの当たり判定が必要かの確認
	if (!(isATargetingB || isBTargetingA)) { //!< どちらも必要ではない場合
		return;
	}

	// どちらか片方が当たり判定を必要としてた場合
	bool isCollision = CollisionDetection::CheckCollision(
		colliderA->GetColliderPosition(), colliderA->GetBounding(),
		colliderB->GetColliderPosition(), colliderB->GetBounding()
	);

	if (isCollision) { //!< 衝突してた場合
		if (isATargetingB) { //!< Aが当たり判定を必要としてた場合
			colliderA->OnCollision(colliderB);
		}

		if (isBTargetingA) { //!< Bが当たり判定を必要としてた場合
			colliderB->OnCollision(colliderA);
		}
	}
}

void ColliderManager::DrawCollider(const Collider* const collider) const {

	const auto& bounding = collider->GetBounding();

	//!< boundingの型の取得
	if (std::holds_alternative<CollisionBoundings::Sphere>(bounding)) {
		DrawSphereCollider(collider->GetColliderPosition(), std::get<CollisionBoundings::Sphere>(bounding));

	} else if (std::holds_alternative<CollisionBoundings::AABB>(bounding)) {
		DrawAABBCollider(collider->GetColliderPosition(), std::get<CollisionBoundings::AABB>(bounding));

	}
}

void ColliderManager::DrawSphereCollider(const Vector3f& position, const CollisionBoundings::Sphere& sphere) const {

	const uint32_t kSubdivision = 24; //!< parameter
	const float kRoundEvery = pi_v * 2.0f / kSubdivision; //!< 1周 / 分割数

	// xz軸の円
	for (uint32_t lon = 0; lon < kSubdivision; ++lon) {
		float theta = kRoundEvery * lon;

		// 単位円の生成
		Vector3f start = { //!< 始点
			std::sin(theta),
			0.0f,
			std::cos(theta),
		};

		Vector3f end = { //!< 終点
			std::sin(theta + kRoundEvery),
			0.0f,
			std::cos(theta + kRoundEvery),
		};

		// 半径を使ってローカル座標での円座標にする
		start *= sphere.radius;
		end   *= sphere.radius;

		SxavengerGame::DrawLine(start + position, end + position, color_);
	}

	// xy軸の円
	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float phi = kRoundEvery * lat;

		// 単位円の生成
		Vector3f start = { //!< 始点
			std::cos(phi),
			std::sin(phi),
			0.0f
		};

		Vector3f end = { //!< 終点
			std::cos(phi + kRoundEvery),
			std::sin(phi + kRoundEvery),
			0.0f
		};

		// 半径を使ってローカル座標での円座標にする
		start *= sphere.radius;
		end *= sphere.radius;

		SxavengerGame::DrawLine(start + position, end + position, color_);
	}

	// yz軸の円
	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float phi = kRoundEvery * lat;

		// 単位円の生成
		Vector3f start = { //!< 始点
			0.0f,
			std::sin(phi),
			std::cos(phi)
		};

		Vector3f end = { //!< 終点
			0.0f,
			std::sin(phi + kRoundEvery),
			std::cos(phi + kRoundEvery),
		};

		// 半径を使ってローカル座標での円座標にする
		start *= sphere.radius;
		end *= sphere.radius;

		SxavengerGame::DrawLine(start + position, end + position, color_);
	}
}

void ColliderManager::DrawAABBCollider(const Vector3f& position, const CollisionBoundings::AABB& aabb) const {

	Vector3f pos[8] = {};

	// A
	for (int i = 0; i < 4; ++i) {
		pos[i] = aabb.localMin;
	}

	pos[1].z = aabb.localMax.z;
	pos[2].y = aabb.localMax.y;
	pos[2].z = aabb.localMax.z;
	pos[3].y = aabb.localMax.y;

	// B
	for (int i = 4; i < 8; ++i) {
		pos[i] = aabb.localMax;
	}

	pos[4].y = aabb.localMin.y;
	pos[4].z = aabb.localMin.z;
	pos[5].y = aabb.localMin.y;
	pos[7].z = aabb.localMin.z;

	for (int i = 0; i < 4; ++i) {
		int next = (i + 1) % 4;
		SxavengerGame::DrawLine(pos[i] + position, pos[next] + position, color_);
		SxavengerGame::DrawLine(pos[i + 4] + position, pos[next + 4] + position, color_);
		SxavengerGame::DrawLine(pos[i] + position, pos[i + 4] + position, color_);
	}
}
