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

	meshes_.resize(model_->GetMeshSize());
	
	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		meshes_[i] = std::make_unique<Mesh>(model_->GetMesh(i).GetVertexBuffer(), model_->GetMesh(i).GetIndexBuffer());
	}
}

void Kipfel::Term() {
}

void Kipfel::Update() {
}
