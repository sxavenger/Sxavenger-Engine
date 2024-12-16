#include "ColliderCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "CollisionDetection.h"

//* engine
#include <Engine/Module/SxavengerModule.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderCollection::CheckCollision() {
	SetupColliderState();
	CheckAllCollision();
	CallBack();
}

void ColliderCollection::Draw() {
	for (const auto& collider : colliders_) {

		Color4f color = inactiveColor_;

		if (collider->IsActive()) {
			color = activeColor_;
		}

		ColliderDrawer::DrawCollider(collider, color);
	}
}

void ColliderCollection::SystemDebugGui() {
	ImGui::ColorEdit4("active",   &activeColor_.r);
	ImGui::ColorEdit4("inactive", &inactiveColor_.r);
}

void ColliderCollection::SetCollider(Collider* collider) {
	colliders_.emplace(collider);
}

void ColliderCollection::EraseCollider(Collider* collider) {
	colliders_.erase(collider);
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
// ColliderDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ColliderDrawer::DrawCollider(const Collider* collider, const Color4f& color) {

	const std::optional<CollisionBoundings::Boundings>& bounding = collider->GetBoundings();

	if (!bounding.has_value()) {
		return;
	}

	Visitor visitor = {};
	visitor.position = collider->GetPosition();
	visitor.color    = color;

	std::visit(visitor, bounding.value());
}

void ColliderDrawer::DrawSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) {

	static const uint32_t kSubdivision = 24;                     //!< parameter
	static const float kRoundEvery = pi_v * 2.0f / kSubdivision; //!< 1周 / 分割数

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

		SxavengerModule::DrawLine(start + position, end + position, color);
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

		SxavengerModule::DrawLine(start + position, end + position, color);
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

		SxavengerModule::DrawLine(start + position, end + position, color);
	}

}

void ColliderDrawer::DrawCapsule(const Vector3f& position, const CollisionBoundings::Capsule& capsule, const Color4f& color) {

	static const uint32_t kSubdivision = 24;

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
		SxavengerModule::DrawLine(currentPoint + topCenter, nextPoint + topCenter, color);
		SxavengerModule::DrawLine(currentPoint + bottomCenter, nextPoint + bottomCenter, color);
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
		SxavengerModule::DrawLine((xAxis * x + capsule.direction * y) + topCenter, (xAxis * nextX + capsule.direction * nextY) + topCenter, color);
		SxavengerModule::DrawLine((xAxis * x - capsule.direction * y) + bottomCenter, (xAxis * nextX - capsule.direction * nextY) + bottomCenter, color);
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
		SxavengerModule::DrawLine((zAxis * z + capsule.direction * y) + topCenter, (zAxis * nextZ + capsule.direction * nextY) + topCenter, color);
		SxavengerModule::DrawLine((zAxis * z - capsule.direction * y) + bottomCenter, (zAxis * nextZ - capsule.direction * nextY) + bottomCenter, color);
	}

	SxavengerModule::DrawLine(xAxis * capsule.radius + topCenter, xAxis * capsule.radius + bottomCenter, color);
	SxavengerModule::DrawLine(-xAxis * capsule.radius + topCenter, -xAxis * capsule.radius + bottomCenter, color);
	SxavengerModule::DrawLine(zAxis * capsule.radius + topCenter, zAxis * capsule.radius + bottomCenter, color);
	SxavengerModule::DrawLine(-zAxis * capsule.radius + topCenter, -zAxis * capsule.radius + bottomCenter, color);
}

void ColliderDrawer::DrawAABB(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color) {

	Vector3f pos[8] = {};

	// A
	for (int i = 0; i < 4; ++i) {
		pos[i] = aabb.min;
	}

	pos[1].z = aabb.max.z;
	pos[2].y = aabb.max.y;
	pos[2].z = aabb.max.z;
	pos[3].y = aabb.max.y;

	// B
	for (int i = 4; i < 8; ++i) {
		pos[i] = aabb.max;
	}

	pos[4].y = aabb.min.y;
	pos[4].z = aabb.min.z;
	pos[5].y = aabb.min.y;
	pos[7].z = aabb.min.z;

	for (int i = 0; i < 4; ++i) {
		int next = (i + 1) % 4;
		SxavengerModule::DrawLine(pos[i] + position, pos[next] + position, color);
		SxavengerModule::DrawLine(pos[i + 4] + position, pos[next + 4] + position, color);
		SxavengerModule::DrawLine(pos[i] + position, pos[i + 4] + position, color);
	}
}

void ColliderDrawer::DrawOBB(const Vector3f& position, const CollisionBoundings::OBB& obb, const Color4f& color) {

	Vector3f pos[8] = {};

	// sizeのみの座標に設定
	pos[0] = { -obb.size.x, obb.size.y, -obb.size.z };
	pos[4] = { -obb.size.x, obb.size.y, obb.size.z };

	pos[1] = { -obb.size.x, -obb.size.y, -obb.size.z };
	pos[5] = { -obb.size.x, -obb.size.y, obb.size.z };

	pos[2] = { obb.size.x, -obb.size.y, -obb.size.z };
	pos[6] = { obb.size.x, -obb.size.y, obb.size.z };

	pos[3] = { obb.size.x, obb.size.y, -obb.size.z };
	pos[7] = { obb.size.x, obb.size.y, obb.size.z };

	for (int i = 0; i < 8; ++i) {
		pos[i] = Matrix::Transform(pos[i], obb.orientation * Matrix::MakeTranslate(position)); //!< 姿勢行列 + centerの適用
	}

	for (int i = 0; i < 4; ++i) {
		int next = (i + 1) % 4;
		SxavengerModule::DrawLine(pos[i], pos[next], color);
		SxavengerModule::DrawLine(pos[i + 4], pos[next + 4], color);
		SxavengerModule::DrawLine(pos[i], pos[i + 4], color);
	}
}

