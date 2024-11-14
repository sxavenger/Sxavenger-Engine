#include "AnimationPlayer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>


////////////////////////////////////////////////////////////////////////////////////////////
// AnimationPlayer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationPlayer::Init() {
	SetName("animation player");

	model_ = SxavengerGame::LoadModel("resources/model", "player_jump.gltf");
	CreateAnimation();

	AnimationBehavior::renderingFlag_ = kBehaviorRender_Systematic;

	test_curve.TEST_Create();

	test_ = std::make_unique<Collider>();
	test_->SetColliderBoundingSphere({ 4.0f });

	test2_ = std::make_unique<Collider>();
	test2_->SetColliderBoundingAABB();
}

void AnimationPlayer::Term() {
}

void AnimationPlayer::Update() {
	AnimationBehavior::animationTime_.AddDeltaTime();
	UpdateAnimator(0, true);
	//UpdateAnimator(0, true);

	test_->SetColliderPosition(transform_.GetWorldPosition());
	test2_->SetColliderPosition(transform_.GetWorldPosition());
}

void AnimationPlayer::SetAttributeImGui() {
	test_curve.SetImGuiCommand();
}
