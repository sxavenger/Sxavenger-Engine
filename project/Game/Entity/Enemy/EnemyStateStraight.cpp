#include "EnemyStateStraight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateStraight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateStraight::Init() {
	duration_ = enemy_->animators_[Enemy::AnimationState::Straight]->GetDurationTime(0);
	enemy_->SetAnimationState(Enemy::AnimationState::Straight);
}

void EnemyStateStraight::Term() {
}

void EnemyStateStraight::Update() {
	time_ += SxavengerSystem::GetDeltaTime();
	enemy_->time_ = time_;

	if (time_ >= duration_) {
		enemy_->requestState_ = std::make_unique<EnemyStateRoot>(enemy_);
	}
}
