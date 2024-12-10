#include "Weapon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Weapon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Weapon::Init() {

	model_ = SxavengerAsset::ImportModel("asset/model/demo/sphere.obj");
	model_->AsyncLoad();
	SxavengerSystem::PushTask(model_);
	model_->WaitCompleted();

	ModelBehavior::model_ = model_.get();
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	SetName("weapon");

	isActive_ = false;

	TransformComponent::GetTransform().translate = { 0.0f, 0.0f, 2.0f };
	TransformComponent::UpdateMatrix();
}

void Weapon::Term() {
}

void Weapon::Update() {
}
