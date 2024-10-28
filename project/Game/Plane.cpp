#include "Plane.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

void Plane::Init() {
	SetName("plane");

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "plane.obj");
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	renderingFlag_ = kBehaviorRender_Raytracing | kBehaviorRender_Systematic;
}

void Plane::SetAttributeImGui() {
}
