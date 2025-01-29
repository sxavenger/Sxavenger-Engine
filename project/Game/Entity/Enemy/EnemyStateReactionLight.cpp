#include "EnemyStateReactionLight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* enemy
#include "Enemy.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateReactionLight class
////////////////////////////////////////////////////////////////////////////////////////////

void EnemyStateReactionLight::Init() {
	duration_ = enemy_->animators_[Enemy::AnimationState::ReactionLight]->GetDurationTime(0);
	enemy_->SetAnimationState(Enemy::AnimationState::ReactionLight);
}

void EnemyStateReactionLight::Term() {
}

void EnemyStateReactionLight::Update() {
	time_ += SxavengerSystem::GetDeltaTime();
	enemy_->time_ = time_;

	if (time_ >= duration_) {
		enemy_->requestState_ = std::make_unique<EnemyStateRoot>(enemy_);
	}
}
