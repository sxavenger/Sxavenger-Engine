#include "Kipfel.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Kipfel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Kipfel::Init() {

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/kipfel", "kipfel.fbx");
	//ModelBehavior::model_->ApplyMeshShader();
	model_->ApplyRaytracing();

	renderingFlag_ = kBehaviorRender_Raytracing;
	CreateRaytracingRecorder();

	SetName("kipfel");
}

void Kipfel::Term() {
}

void Kipfel::Update() {
}
