#include "Ground.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Ground class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Ground::Init() {
	model_ = SxavengerAsset::ImportModel("asset/model/demo/room.obj");
	model_->AsyncLoad();
	SxavengerSystem::PushTask(model_);

	ModelBehavior::model_ = model_.get();
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	SetName("Ground");
}

void Ground::Term() {
}

void Ground::Update() {
}

void Ground::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_->IsCompleted()) {
		ModelBehavior::DrawSystematic(frame);
	}
}

void Ground::SetAttributeImGui() {
}
