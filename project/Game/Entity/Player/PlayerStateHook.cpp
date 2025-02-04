#include "PlayerStateHook.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* player
#include "Player.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerStateHooking class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlayerStateHook::Init() {
	duration_ = player_->animators_[Player::AnimationState::Hooking].WaitGet()->GetDurationTime(0);
	player_->SetAnimationState(Player::AnimationState::Hooking);

	attackCollider_ = std::make_unique<PlayerAttackCollider>();
	attackCollider_->SetIsActive(false);
	attackCollider_->SetToCollection();
	attackCollider_->SetTypeId(ColliderType::kPlayerAttack);
	attackCollider_->SetTargetId(ColliderType::kEnemy);
	attackCollider_->SetColliderBoundingSphere({ 0.4f });
	attackCollider_->SetParent(player_);
	attackCollider_->GetTransform().translate = { 0.0f, 1.2f, 0.8f };
	attackCollider_->UpdateMatrix();
	attackCollider_->strength_ = AttackStrength::Light;
	attackCollider_->SetOnCollisionEnter([this](_MAYBE_UNUSED Collider* const) {
		player_->SetShake(0.1f, { 0.2f, 0.1f });
	});

}

void PlayerStateHook::Term() {
}

void PlayerStateHook::Update() {
	ActionGamepad();
	UpdateAnimation();

	if (time_ >= duration_) {
		if (nextAttackState_.has_value()) {
			player_->requestState_ = std::move(nextAttackState_.value());

		} else {
			player_->requestState_ = std::make_unique<PlayerStateRoot>(player_);
		}

	} else if (time_ >= duration_.time * 0.2f) {
		attackCollider_->SetIsActive(true);
	}
}

void PlayerStateHook::ActionGamepad() {
	if (!player_->gamepad_->IsConnect()) {
		return; //!< gamepadが接続されていない
	}

	if (nextAttackState_.has_value()) {
		return;
	}

	if (player_->gamepad_->IsTrigger(GamepadButtonId::BUTTON_B)) {
		nextAttackState_ = std::make_unique<PlayerStateElbow>(player_);
	}
}

void PlayerStateHook::UpdateAnimation() {
	time_ += SxavengerSystem::GetDeltaTime().time * 1.4f;

	//* player側に設定
	player_->time_ = time_;
}
