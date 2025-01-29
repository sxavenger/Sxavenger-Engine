#include "PlayerStatePunch.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStatePunch class
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStatePunch::Init() {
	duration_ = player_->animators_[Player::AnimationState::Punching].WaitGet()->GetDurationTime(0);
	player_->SetAnimationState(Player::AnimationState::Punching);

	attackCollider_ = std::make_unique<PlayerAttackCollider>();
	attackCollider_->SetToCollection();
	attackCollider_->SetTypeId(ColliderType::kPlayerAttack);
	attackCollider_->SetTargetId(ColliderType::kEnemy);
	attackCollider_->SetColliderBoundingSphere({ 0.4f });
	attackCollider_->SetParent(player_);
	attackCollider_->GetTransform().translate = { 0.0f, 1.2f, 0.8f };
	attackCollider_->UpdateMatrix();
	attackCollider_->strength_ = AttackStrength::Light;
	attackCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const) {
		player_->SetShake(0.1f, { 0.1f, 0.1f });
	});
}

void PlayerStatePunch::Term() {
}

void PlayerStatePunch::Update() {
	ActionGamepad();
	UpdateAnimation();

	if (time_ >= duration_) {
		if (nextAttackState_.has_value()) {
			player_->requestState_ = std::move(nextAttackState_.value());

		} else {
			player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
		}
	}

}

void PlayerStatePunch::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateHook>(player_);
	}
}

void PlayerStatePunch::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime().time * 1.4f;

	//* player側に設定
	player_->time_ = time_;
}
