#include "EnemyStateKnock.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateKnock class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateKnock::Init() {
	duration_ = enemy_->animators_[Enemy::AnimationState::Knock]->GetDurationTime(0);
	enemy_->SetAnimationState(Enemy::AnimationState::Knock);

	enemy_->hitCollider_->SetIsActive(false);
}

void EnemyStateKnock::Term() {
}

void EnemyStateKnock::Update() {

	time_ += SxavengerSystem::GetDeltaTime();

	if (time_ >= duration_) {
		enemy_->isDead_ = true;
	}

	time_ = Min(time_, { duration_.time - 0.01f });

	enemy_->time_ = time_;

}
