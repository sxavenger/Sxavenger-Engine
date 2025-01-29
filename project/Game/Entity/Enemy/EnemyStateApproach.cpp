#include "EnemyStateApproach.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

//* lib
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateApproach::Init() {
	enemy_->SetAnimationState(Enemy::AnimationState::Approach);

	approachCollider_ = std::make_unique<Collider>();
	approachCollider_->SetToCollection();
	approachCollider_->SetColliderBoundingSphere({ 0.4f });
	approachCollider_->SetParent(enemy_);
	approachCollider_->GetTransform().translate = { 0.0f, 1.0f, 0.2f };
	approachCollider_->UpdateMatrix();

	approachCollider_->SetTypeId(ColliderType::kEnemyApproach);
	approachCollider_->SetTargetId(ColliderType::kPlayer);
	approachCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const target) {
			enemy_->requestState_ = std::make_unique<EnemyStateStraight>(enemy_);
		}
	);
}

void EnemyStateApproach::Term() {
}

void EnemyStateApproach::Update() {
	enemy_->time_ += SxavengerSystem::GetDeltaTime();

	if (enemy_->target_ == nullptr) {
		enemy_->requestState_ = std::make_unique<EnemyStateRoot>(enemy_);
	}

	Vector3f position = { enemy_->GetPosition().x, 0.0f, enemy_->GetPosition().z };
	Vector3f targetPosition = { enemy_->target_->GetPosition().x, 0.0f, enemy_->target_->GetPosition().z };

	//* 方向を求める
	Vector3f direction = CalculateEuler(Normalize(targetPosition - position));
	enemy_->GetTransform().rotate = ToQuaternion(direction);

	static const Vector3f base = { 0.0f, 0.0f, speed_ };
	Vector3f move = RotateVector(base, enemy_->GetTransform().rotate);

	//* enemyの移動
	enemy_->GetTransform().translate += move * SxavengerSystem::GetDeltaTime().time;
	approachCollider_->UpdateMatrix();
}
