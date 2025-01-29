#include "PlayerStateKick.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateRolling class
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateKick::Init() {
	duration_ = player_->animators_[Player::AnimationState::Kick].WaitGet()->GetDurationTime(0);
	player_->SetAnimationState(Player::AnimationState::Kick);

	attackCollider_ = std::make_unique<PlayerAttackCollider>();
	attackCollider_->SetToCollection();
	attackCollider_->SetTypeId(ColliderType::kPlayerAttack);
	attackCollider_->SetTargetId(ColliderType::kEnemy);
	attackCollider_->SetColliderBoundingSphere({ 0.4f });
	attackCollider_->SetParent(player_);
	attackCollider_->GetTransform().translate = { 0.0f, 1.2f, 0.8f };
	attackCollider_->UpdateMatrix();
	attackCollider_->strength_ = AttackStrength::Heavy;
	attackCollider_->damage_   = 2.0f;
	attackCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const) {
		player_->SetShake(1.0f, { 0.4f, 0.2f });
	});
}

void PlayerStateKick::Term() {
}

void PlayerStateKick::Update() {
	UpdateAnimation();

	if (time_ >= duration_) {
		player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
	}
}

void PlayerStateKick::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime();

	//* player側に設定
	player_->time_ = time_;
}
