#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Components/Entity/BehaviourHelper.h>
#include <Engine/Module/Parameter/SerializeGui.h>

SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Player::Load() {
	model_ = sContentStorage->Import<SxxEngine::ContentModel>(
		"Assets/models/human/idle.gltf"
	);

	animations_[static_cast<uint8_t>(AnimationType::Idle)]
		= sContentStorage->Import<ContentAnimation>("assets/models/human/idle.gltf")->GetAnimation(0);

	animations_[static_cast<uint8_t>(AnimationType::Walk)]
		= sContentStorage->Import<ContentAnimation>("assets/models/human/walking.gltf")->GetAnimation(0);

	animations_[static_cast<uint8_t>(AnimationType::Dash)]
		= sContentStorage->Import<ContentAnimation>("assets/models/human/dash.gltf")->GetAnimation(0);
}

void Player::Awake() {

	BehaviourHelper::CreateSkinnedMeshBehaviour(address_, model_.Get());

	(*this)->SetName("Player");
	(*this)->AddComponent<TransformComponent>();

	// TODO: Colliderの追加
	UpdateAnimation();
}

void Player::Update() {

	Move();
	UpdateAnimation();

}

void Player::Inspectable() {
	SerializeGuiFormatter<float>::DragScalar(walkspeed_, 0.01f, 0.0f, std::nullopt);
	SerializeGuiFormatter<float>::DragScalar(dashSpeed_, 0.01f, 0.0f, std::nullopt);
}

void Player::SetIdle() {
	SetAnimationState(AnimationType::Idle);
	velocity_ = kOrigin3<float>;
}

void Player::SetWalk(const Vector3f& velocity) {
	SetAnimationState(AnimationType::Walk);
	velocity_ = velocity.Normalize() * (*walkspeed_);
}

void Player::SetDash(const Vector3f& velocity) {
	SetAnimationState(AnimationType::Dash);
	velocity_ = velocity.Normalize() * (*dashSpeed_);
}

void Player::SetAnimationState(AnimationType type) {
	if (currentAnimationState_.type == type) {
		return; //!< 変更なし
	}

	//!< prevに設定
	prevAnimationState_ = currentAnimationState_;

	//!< currentを初期化
	currentAnimationState_.type = type;
	currentAnimationState_.time.Reset();

	//!< transition timeを初期化
	animationTransitionTime_.Reset();

}

void Player::UpdateAnimation() {

	currentAnimationState_.time.AddDeltaTime();

	auto currentAnimation
		= animations_[static_cast<uint8_t>(currentAnimationState_.type)].WaitGet();

	if (prevAnimationState_.has_value()) {
		//!< アニメーション遷移

		animationTransitionTime_.AddDeltaTime();

		float t = Saturate(static_cast<float>(animationTransitionTime_.time / 0.5));

		auto prevAnimation
			= animations_[static_cast<uint8_t>((*prevAnimationState_).type)].WaitGet();

		BehaviourHelper::ApplyAnimationTransition(
			address_,
			prevAnimation->GetAnimation(), (*prevAnimationState_).time, true,
			currentAnimation->GetAnimation(), currentAnimationState_.time, true,
			t
		);

		if (t >= 1.0f) {
			prevAnimationState_ = std::nullopt;
		}

	} else {
		//!< 通常再生

		BehaviourHelper::ApplyAnimation(
			address_,
			currentAnimation->GetAnimation(), currentAnimationState_.time, true
		);

	}

}

void Player::Move() {

	auto transform = (*this)->GetComponent<TransformComponent>();
	transform->translate += velocity_ * static_cast<float>(System::GetDeltaTimed().time);

}
