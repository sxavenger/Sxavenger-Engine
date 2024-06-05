#include "Bunny.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Bunny class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Bunny::Init() {
	// IA
	model_ = std::make_unique<Model>("./Resources/model", "bunny.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMeshData(0).vertexResource.get(), model_->GetMeshData(0).indexResource.get(),
		L"bunny"
	);
	
	transform_.scale     = { 2.0f, 2.0f, 2.0f };
	transform_.translate = { 10.0f, 0.0f, -10.0f };

	worldMatrix_ = transform_.SetMatrix();
}

void Bunny::Term() {
}

void Bunny::Update() {
	static float frame = 0.0f; // test param
	frame -= 0.01f;

	transform_.rotate.y = frame;

	worldMatrix_ = transform_.SetMatrix();
}
