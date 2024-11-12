#include "Field.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Field class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Field::Init() {
	ModelBehavior::renderingFlag_ = kBehaviorRender_Systematic | kBehaviorRender_Raytracing;
	ModelBehavior::model_         = SxavengerGame::LoadModel("resources/model", "field_smooth.obj");
	SetName("field");

	model_->ApplyRaytracing();
	CreateRaytracingRecorder();

	ModelBehavior::transform_.transform.scale = { 0.1f, 0.1f, 0.1f };
	ModelBehavior::transform_.UpdateMatrix();

	ModelBehavior::uvTransform_.transform.scale = { 4.0f, 4.0f };
	ModelBehavior::uvTransform_.Transfer();

	ModelBehavior::material_.material.roughness = 0.8f;
	ModelBehavior::material_.material.metallic  = 0.2f;
	ModelBehavior::material_.Transfer();

}

void Field::Term() {
}
