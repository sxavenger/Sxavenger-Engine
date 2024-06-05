#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {
	// IA
	model_ = std::make_unique<Model>("./Resources/model", "teapot.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMeshData(0).vertexResource.get(), model_->GetMeshData(0).indexResource.get(),
		L"teapot"
	);

	transform_.scale     *= 2.0f;
	transform_.translate = { 0.0f, 1.6f, 0.0f };

	worldMatrix_ = transform_.SetMatrix();
}

void Teapot::Term() {
}
