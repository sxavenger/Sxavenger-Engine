#include "EnemyStateRoot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateRoot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateRoot::Init() {
	enemy_->SetAnimationState(Enemy::AnimationState::FightingIdle);

	viewCollder_ = std::make_unique<Collider>();
	viewCollder_->SetToCollection();
	viewCollder_->SetColliderBoundingCapsule({ { 0.0f, 0.0f, 1.0f }, 1.0f, 1.0f });
	viewCollder_->SetParent(enemy_);
	viewCollder_->GetTransform().translate = { 0.0f, 1.0f, 1.0f };
	viewCollder_->UpdateMatrix();

	viewCollder_->SetTypeId(ColliderType::kEnemyApproach);
	viewCollder_->SetTargetId(ColliderType::kPlayer);
	viewCollder_->SetOnCollisionEnter([this](Collider* const target) {
		enemy_->SetTarget(target);
	});
}

void EnemyStateRoot::Term() {
}

void EnemyStateRoot::Update() {
	enemy_->time_ += SxavengerSystem::GetDeltaTime();

	if (enemy_->target_ != nullptr) {
		enemy_->requestState_ = std::make_unique<EnemyStateApproach>(enemy_);
	}
}
