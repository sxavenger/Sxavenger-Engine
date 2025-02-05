#include "PlayerStateElbow.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateElbow class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateElbow::Init() {
	duration_ = player_->animators_[Player::AnimationState::Elbow].WaitGet()->GetDurationTime(0);
	player_->SetAnimationState(Player::AnimationState::Elbow);

	attackCollider_ = std::make_unique<PlayerAttackCollider>();
	attackCollider_->SetIsActive(false);
	attackCollider_->SetToCollection();
	attackCollider_->SetTypeId(ColliderType::kPlayerAttack);
	attackCollider_->SetTargetId(ColliderType::kEnemy);
	attackCollider_->SetColliderBoundingSphere({ 0.4f });
	attackCollider_->SetParent(player_);
	attackCollider_->GetTransform().translate = { 0.2f, 1.2f, 0.8f };
	attackCollider_->UpdateMatrix();
	attackCollider_->strength_ = AttackStrength::Light;
	attackCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const) {
		player_->SetShake(0.1f, { 0.1f, 0.2f });
		player_->delta_ = 0.2f;
	});

}

void PlayerStateElbow::Term() {
}

void PlayerStateElbow::Update() {
	UpdateAnimation();
	ActionGamepad();

	if (time_ >= duration_.time / 2.0f) {
		attackCollider_->SetIsActive(true);
	}

	if (time_ >= duration_) {
		if (nextAttackState_.has_value()) {
			player_->requestState_ = std::move(nextAttackState_.value());
			
		} else {
			player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
		}
	}
}

void PlayerStateElbow::ImGuiCommand() {

}

void PlayerStateElbow::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateKick>(player_);
	}
}

void PlayerStateElbow::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime().time * player_->delta_;

	//* player側に設定
	player_->time_ = time_;
}
