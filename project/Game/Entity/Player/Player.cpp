#include "Player.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/SxavengerModule.h>

//* lib
#include <Lib/MyMath.h>
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Player class methods
////////////////////////////////////////////////////////////////////////////////////////////
void Player::Init() {

	//* external *//

	keyboard_ = SxavengerSystem::GetInput()->GetKeyboardInput();
	gamepad_  = SxavengerSystem::GetInput()->GetGamepadInput(0);

	//camera_ = sConsole->GetGameCamera();

	//* state *//

	state_ = std::make_unique<PlayerStateRoot>(this);
	state_->Init();

	AModelAnimationActor::SetName("player");
	AModelAnimationActor::SetTransparency(AGeometryActor::Transparency::Opaque);

	AModelAnimationActor::SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/sample/idle.gltf"));
	AModelAnimationActor::Init();
	AModelAnimationActor::GetSkeleton()->CreateBottomLevelAS(SxavengerSystem::GetMainThreadContext());

	//* animation *//

	animators_[AnimationState::Idle]     = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/idle.gltf");
	animators_[AnimationState::Walking]  = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/walking.gltf");
	animators_[AnimationState::Running]  = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/running.gltf");
	animators_[AnimationState::Rolling]  = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/rolling.gltf");
	animators_[AnimationState::Punching] = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/punching.gltf");
	animators_[AnimationState::Hooking]  = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/hooking.gltf");
	animators_[AnimationState::Elbow]    = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/elbow.gltf");
	animators_[AnimationState::Kick]     = SxavengerAsset::TryImport<AssetAnimator>("asset/model/sample/kick.gltf");

	animationState_ = AnimationState::Idle;

	//* collider *//

	hitCollider_ = std::make_unique<Collider>();
	hitCollider_->SetToCollection();

	hitCollider_->SetColliderBoundingCapsule({
		{0.0f, 1.0f, 0.0f}, //!< direction
		0.3f, 1.0f
	});
	hitCollider_->SetParent(this);
	hitCollider_->GetTransform().translate = { 0.0f, 1.0f, 0.0f };
	hitCollider_->SetTypeId(ColliderType::kPlayer);

	//dof_ = std::make_unique<VisualDoF>();
	//dof_->Init();
	//dof_->SetToConsole();
	//dof_->GetParameter().f           = 30.0f;
	//dof_->GetParameter().focusLength = 5.0f;


	/*texture_ = SxavengerAsset::TryImport<Texture>("asset/textures/ui.png").Lock();
	SxavengerAsset::PushTask(texture_);
	texture_->WaitComplete();*/
}

void Player::Init(const QuaternionTransform& transform) {
	Init();
	AModelAnimationActor::GetTransform() = transform;
	AModelAnimationActor::UpdateMatrix();
	hitCollider_->UpdateMatrix();
	UpdateCamera();
}

void Player::Term() {
}

void Player::Update() {
	UpdateState();

	AModelAnimationActor::GetTransform().translate.x = std::clamp(AModelAnimationActor::GetTransform().translate.x, -11.0f, 11.0f);
	AModelAnimationActor::GetTransform().translate.z = std::clamp(AModelAnimationActor::GetTransform().translate.z, -11.0f, 11.0f);
	AModelAnimationActor::UpdateMatrix();
	hitCollider_->UpdateMatrix();
	UpdateCamera();

	//* update skeleton *//

	UpdateAnimation();

	delta_ = 1.0f;
	if (delta_ < 1.0f) {
		delta_ += SxavengerSystem::GetDeltaTime().time / 1.5f;
		delta_ = std::clamp(delta_, 0.0f, 1.0f);
	}
}

void Player::SetAnimationState(AnimationState state) {
	if (state != animationState_) {
		prevAnimationState_ = animationState_;
		prevTime_           = time_;

		animationT_ = 0.0f;

		animationState_ = state;
		time_           = {};
	}
}

void Player::AttributeImGui() {
	if (ImGui::TreeNode("collider")) {
		hitCollider_->SetImGuiCommand();
		ImGui::TreePop();
	}
}

//void Player::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
//	SxavengerModule::GetSpriteCommon()->DrawSprite(
//		{}, kMainWindowSize, texture_->GetGPUHandleSRV()
//	);
//}

void Player::SetShake(TimePointf<TimeUnit::second> time, const Vector2f& strength) {
	shakeTime_  = time;
	shakeTimer_ = {};

	strength_ = strength;
}

void Player::UpdateState() {
	//!< apply request state
	if (requestState_.has_value()) {
		state_->Term(); //!< 前のstateを終了

		state_ = std::move(requestState_.value()); //!< request stateを適用
		state_->Init();

		requestState_ = std::nullopt;
	}

	//!< update state
	state_->Update();
}

void Player::UpdateAnimation() {
	if (animationT_ < 1.0f) {
		animationT_ += SxavengerSystem::GetDeltaTime().time / 0.2f;
		animationT_ = std::clamp(animationT_, 0.0f, 1.0f);

		AModelAnimationActor::GetSkeleton()->UpdateTransitionAnimation(
			animators_[prevAnimationState_].WaitGet()->GetAnimation(0), prevTime_, true,
			animators_[animationState_].WaitGet()->GetAnimation(0), time_, true,
			animationT_
		);

	} else {
		AModelAnimationActor::GetSkeleton()->UpdateAnimation(animators_[animationState_].WaitGet()->GetAnimation(0), time_);
	}

	AModelAnimationActor::GetSkeleton()->UpdateBottomLevelAS(SxavengerSystem::GetMainThreadContext());
}

void Player::UpdateCamera() {
	pivot_ = GetPosition();
	pivotRotation_.x = std::fmod(pivotRotation_.x, pi_v * 2.0f);
	pivotRotation_.y = std::clamp(pivotRotation_.y, 0.0f, pi_v / 14.0f);

	Quaternion rotate
		= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, pivotRotation_.x)
		* MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, pivotRotation_.y);

	static const Vector3f direction = { 0.0f, 0.0f, -1.0f };
	Vector3f rotatedDirection = RotateVector(direction, rotate);

	QuaternionTransform transform = {};
	transform.translate = pivot_ + rotatedDirection * distance_ + offset_;
	transform.rotate    = rotate;

	//* カメラシェイクの処理

	if (shakeTimer_ < shakeTime_) {
		shakeTimer_ += SxavengerSystem::GetDeltaTime();

		float t = std::lerp(1.0f, 0.0f, shakeTimer_.time / shakeTime_.time);
		t = std::clamp(t, 0.0f, 1.0f);

		Vector3f shake = {
			Random::Generate(-strength_.x, strength_.x) * t,
			Random::Generate(-strength_.y, strength_.y) * t,
			0.0f
		};

		shake = RotateVector(shake, rotate);
		transform.translate += shake;
	}

	camera_->GetTransform() = transform;
	camera_->UpdateView();
}

void Player::UpdateParticle() {
}
