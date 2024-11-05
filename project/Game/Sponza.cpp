#include "Sponza.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>



void Sponza::Init() {
	SetName("sponza");

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/sponza", "NewSponza_Main_Yup_003.fbx");
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	//transform_.transform.scale = { 16.0f, 16.0f, 16.0f };
	transform_.UpdateMatrix();

	renderingFlag_ = kBehaviorRender_Raytracing;
}

void Sponza::SetAttributeImGui() {
}
