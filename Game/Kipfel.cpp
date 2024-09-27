#include "Kipfel.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Kipfel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Kipfel::Init() {

	model_ = std::make_unique<Model>("resources/model/kipfel", "kipfel.fbx");
	ModelBehavior::model_ = model_.get();

	model_->ApplyMeshShader();

	SetToConsole("kipfel");
}

void Kipfel::Term() {
}

void Kipfel::Update() {
}
