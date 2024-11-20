#include "Plane.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Plane class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Plane::Init() {
	SetName("plane");
	TryLoadJson();

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "plane.obj");
	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	renderingFlag_ = kBehaviorRender_Raytracing | kBehaviorRender_Systematic;

	imguiJsonExporter_.SetName("plane");
	//imguiJsonExporter_.TryLoad();
}

void Plane::Term() {
	//imguiJsonExporter_.Output();
}

void Plane::SetAttributeImGui() {
	imguiJsonExporter_.DragFloat("value", &value_, 0.2f);
}
