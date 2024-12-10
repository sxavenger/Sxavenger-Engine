#include "AnimationDemo.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationDemo class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationDemo::Init() {
	model_ = SxavengerAsset::ImportModel("asset/model/sample/enemy_damage.gltf");
	model_->AsyncLoad();
	SxavengerSystem::PushTask(model_);
	model_->WaitCompleted();

	group_.Load("asset/model/sample/enemy_damage.gltf");
	mesh_.Create(model_.get());

	SetName("animation demo");

	AnimationBehavior::model_        = model_.get();
	AnimationBehavior::skeletonMesh_ = &mesh_;

	AnimationBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	chess_ = std::make_unique<ChessBoard>();
	chess_->Init();
	SetChild(chess_.get());
}

void AnimationDemo::Term() {
}

void AnimationDemo::Update() {

	time_.AddDeltaTime();
	mesh_.UpdateAnimation(group_, time_);
}

void AnimationDemo::SetAttributeImGui() {
	ImGui::Checkbox("active", &isActive_);
}
