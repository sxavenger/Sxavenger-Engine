#include "AnimationPlayer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Console/SystemConsole.h>


////////////////////////////////////////////////////////////////////////////////////////////
// AnimationPlayer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationPlayer::Init() {
	SetName("animation player");

	model_ = SxavengerGame::LoadModel("resources/model", "player_jump.gltf");

	animator_ = std::make_unique<Animator>(model_);
	AnimationBehavior::animator_ = animator_.get();

	AnimationBehavior::renderingFlag_ = kBehaviorRender_Systematic;

	test_curve.TEST_Create();

	testA_ = std::make_unique<Collider>();
	testA_->SetColliderBoundingCapsule({ .direction = { 1.0f, 0.0f, 0.0f }, .radius = {1.0f}, .length = { 2.0f }});
	testA_->SetTypeId(ColliderTypeTest_A);
	testA_->SetTargetTypeId(ColliderTypeTest_B);

	testB_ = std::make_unique<Collider>();
	testB_->SetColliderBoundingCapsule({ .direction = { 1.0f, 0.0f, 0.0f }, .radius = {1.0f}, .length = { 2.0f } });
	testB_->SetTypeId(ColliderTypeTest_B);
	testB_->SetTargetTypeId(ColliderTypeTest_A);

	testB_->SetOnCollisionEnter([&](_MAYBE_UNUSED Collider* const other) {
		sSystemConsole->Log("enter.");
	});

	testB_->SetOnCollisionExit([&](_MAYBE_UNUSED Collider* const other) {
		sSystemConsole->Log("exit.");
	});
}

void AnimationPlayer::Term() {
}

void AnimationPlayer::Update() {
	animationTime_.AddDeltaTime();
	animator_->Update(animationTime_, 0, true);

	testA_->SetColliderPosition(positionA);
	testB_->SetColliderPosition(positionB);
}

void AnimationPlayer::SetAttributeImGui() {
	test_curve.SetImGuiCommand();

	if (ImGui::TreeNode("colliderA")) {
		testA_->SetColliderImGuiCommand();

		if (ImGui::DragFloat3("position", &positionA.x, 0.02f)) {
			testA_->SetColliderPosition(positionA);
		}
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("colliderB")) {
		testB_->SetColliderImGuiCommand();

		if (ImGui::DragFloat3("position", &positionB.x, 0.02f)) {
			testB_->SetColliderPosition(positionB);
		}
		ImGui::TreePop();
	}
}
