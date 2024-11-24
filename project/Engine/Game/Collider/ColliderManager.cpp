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
	SetupCallbackOnCollision(); //!< 当たり判定のsetup
	CheckAllCollision(); //!< 全当たり判定の計算

	for (auto& collider : colliders_) { //!< collider関数の呼び出し
		collider->CallbackOnCollision();
	}
}

void ColliderManager::DrawColliders(const Color4f& color) {
	for (const auto& collider : colliders_) {
		DrawCollider(collider, color);
	}
}

void ColliderManager::SetupCallbackOnCollision() {
	for (auto it = colliders_.begin(); it != colliders_.end(); ++it) {
		(*it)->SetupCallbackOnCollision();
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

	Assert(colliderA->GetColliderPosition().has_value() && colliderB->GetColliderPosition().has_value(), "collider is not set position.");

	// どちらか片方が当たり判定を必要としてた場合
	bool isCollision = CollisionDetection::CheckCollision(
		colliderA->GetColliderPosition().value(), colliderA->GetBounding(),
		colliderB->GetColliderPosition().value(), colliderB->GetBounding()
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

void ColliderManager::DrawCollider(const Collider* const collider, const Color4f& color) const {

	const auto& bounding = collider->GetBounding();

	if (!collider->GetColliderPosition().has_value()) {
		return; //!< collider is not set position.
	}

	const Vector3f& position = collider->GetColliderPosition().value();

	switch (bounding.index()) {
		case CollisionBoundings::BoundingsType::kSphere:
			DrawSphereCollider(
				position, std::get<CollisionBoundings::Sphere>(bounding), color
			);
			break;

		case CollisionBoundings::BoundingsType::kCapsule:
			DrawCapsule(
				position, std::get<CollisionBoundings::Capsule>(bounding), color
			);
			break;

		case CollisionBoundings::BoundingsType::kAABB:
			DrawAABBCollider(
				position, std::get<CollisionBoundings::AABB>(bounding), color
			);
			break;
	}
}

void ColliderManager::DrawSphereCollider(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) const {

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

		SxavengerGame::DrawLine(start + position, end + position, color);
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

		SxavengerGame::DrawLine(start + position, end + position, color);
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

		SxavengerGame::DrawLine(start + position, end + position, color);
	}
}

void ColliderManager::DrawCapsule(const Vector3f& position, const CollisionBoundings::Capsule& capsule, const Color4f& color) const {
	const uint32_t kSubdivision = 24;

	// カプセルの両端の中心を計算
	Vector3f topCenter    = position + capsule.direction * (capsule.length * 0.5f);
	Vector3f bottomCenter = position - capsule.direction * (capsule.length * 0.5f);

	Vector3f arbitrary = (std::abs(capsule.direction.x) < std::abs(capsule.direction.y)) ? Vector3f(1, 0, 0) : Vector3f(0, 1, 0);
	Vector3f xAxis     = Normalize(Cross(capsule.direction, arbitrary)); // 半円の「横」方向
	Vector3f zAxis     = Normalize(Cross(capsule.direction, xAxis));     // 半円の「奥」方向

	const float kLonEvery = pi_v * 2.0f / kSubdivision;
	const float kLatEvery = pi_v / kSubdivision;

	// xz軸円
	for (uint32_t i = 0; i < kSubdivision; ++i) {

		float lon = kLonEvery * i;
		float x = std::cos(lon) * capsule.radius;
		float z = std::sin(lon) * capsule.radius;

		float nextX = std::cos(lon + kLonEvery) * capsule.radius;
		float nextZ = std::sin(lon + kLonEvery) * capsule.radius;

		// 現在の点を計算
		Vector3f currentPoint = xAxis * x + zAxis * z;
		Vector3f nextPoint    = xAxis * nextX + zAxis * nextZ;

		// 線を描画
		SxavengerGame::DrawLine(currentPoint + topCenter, nextPoint + topCenter, color);
		SxavengerGame::DrawLine(currentPoint + bottomCenter, nextPoint + bottomCenter, color);
	}

	// xy軸円
	for (uint32_t i = 0; i < kSubdivision; ++i) {
		float lat = kLatEvery * i;
		float x = std::cos(lat) * capsule.radius;
		float y = std::sin(lat) * capsule.radius;

		float nextX = std::cos(lat + kLatEvery) * capsule.radius;
		float nextY = std::sin(lat + kLatEvery) * capsule.radius;

		// 現在の点を計算
		Vector3f currentPoint = xAxis * x + capsule.direction * y;
		Vector3f nextPoint    = xAxis * nextX + capsule.direction * nextY;

		// 線を描画
		SxavengerGame::DrawLine((xAxis * x + capsule.direction * y) + topCenter, (xAxis * nextX + capsule.direction * nextY) + topCenter, color);
		SxavengerGame::DrawLine((xAxis * x - capsule.direction * y) + bottomCenter, (xAxis * nextX - capsule.direction * nextY) + bottomCenter, color);
	}

	// xy軸円
	for (uint32_t i = 0; i < kSubdivision; ++i) {
		float lat = kLatEvery * i;
		float y = std::sin(lat) * capsule.radius;
		float z = std::cos(lat) * capsule.radius;

		float nextY = std::sin(lat + kLatEvery) * capsule.radius;
		float nextZ = std::cos(lat + kLatEvery) * capsule.radius;

		// 現在の点を計算
		Vector3f currentPoint = zAxis * z + capsule.direction * y;
		Vector3f nextPoint    = zAxis * nextZ + capsule.direction * nextY;

		// 線を描画
		SxavengerGame::DrawLine((zAxis * z + capsule.direction * y) + topCenter, (zAxis * nextZ + capsule.direction * nextY) + topCenter, color);
		SxavengerGame::DrawLine((zAxis * z - capsule.direction * y) + bottomCenter, (zAxis * nextZ - capsule.direction * nextY) + bottomCenter, color);
	}

	SxavengerGame::DrawLine(xAxis * capsule.radius + topCenter, xAxis * capsule.radius + bottomCenter, color);
	SxavengerGame::DrawLine(-xAxis * capsule.radius + topCenter, -xAxis * capsule.radius + bottomCenter, color);
	SxavengerGame::DrawLine(zAxis * capsule.radius + topCenter, zAxis * capsule.radius + bottomCenter, color);
	SxavengerGame::DrawLine(-zAxis * capsule.radius + topCenter, -zAxis * capsule.radius + bottomCenter, color);
}


void ColliderManager::DrawAABBCollider(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color) const {

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
		SxavengerGame::DrawLine(pos[i] + position, pos[next] + position, color);
		SxavengerGame::DrawLine(pos[i + 4] + position, pos[next + 4] + position, color);
		SxavengerGame::DrawLine(pos[i] + position, pos[i + 4] + position, color);
	}
}
