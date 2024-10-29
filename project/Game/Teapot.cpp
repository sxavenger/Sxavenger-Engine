#include "Teapot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {
	SetName("teapot");

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "teapot.obj");
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	renderingFlag_ = kBehaviorRender_Raytracing | kBehaviorRender_Systematic | kBehaviorRender_Adaptive;
}

void Teapot::SetAttributeImGui() {
	ImGui::DragFloat2("pos", &pos_.x, 0.01f);
	ImGui::DragFloat2("size", &size_.x, 0.01f);
	ImGui::DragFloat("rotate", &rotate_, 0.01f);
}

void Teapot::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {
	SxavengerGame::DrawSprite(
		pos_, size_, rotate_, Sxavenger::GetTextureHandleGPU("resources/uvChecker.png"), {1.0f, 1.0f, 1.0f, 1.0f}
	);
}
