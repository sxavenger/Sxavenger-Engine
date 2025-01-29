#include "EnemyStateReactionHeavy.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateReactionLight class
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateReactionHeavy::Init() {
	duration_ = enemy_->animators_[Enemy::AnimationState::ReactionHeavy]->GetDurationTime(0);
	enemy_->SetAnimationState(Enemy::AnimationState::ReactionHeavy);
}

void EnemyStateReactionHeavy::Term() {
}

void EnemyStateReactionHeavy::Update() {
	time_ += SxavengerSystem::GetDeltaTime();
	enemy_->time_ = time_;

	if (time_ >= duration_) {
		enemy_->requestState_ = std::make_unique<EnemyStateRoot>(enemy_);
	}
}
