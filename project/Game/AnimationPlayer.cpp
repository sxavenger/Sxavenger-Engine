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

	model_ = SxavengerGame::LoadModel("resources/model", "player_move_bone.fbx");
	CreateAnimation();

	AnimationBehavior::renderingFlag_ = kBehaviorRender_Systematic;

	test_curve.TEST_Create();
}

void AnimationPlayer::Term() {
}

void AnimationPlayer::Update() {
	AnimationBehavior::animationTime_.AddDeltaTime();
	UpdateAnimator(0, true);
	//UpdateAnimator(0, true);
}

void AnimationPlayer::SetAttributeImGui() {
	test_curve.SetImGuiCommand();
}
