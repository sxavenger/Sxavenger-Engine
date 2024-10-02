#include "Kipfel.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Kipfel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Kipfel::Init() {

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/kipfel", "kipfel.fbx");
	ModelBehavior::model_->ApplyMeshShader();

	SetToConsole("kipfel");
}

void Kipfel::Term() {
}

void Kipfel::Update() {
}
