#include "Ocean.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Ocean class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Ocean::Init() {

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "ocean.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMeshData(0).vertexResource.get(), model_->GetMeshData(0).indexResource.get(),
		L"ocean"
	);

	// transform
	transform_.translate = { -30.0f, 1.0f, 0.0f };
	transform_.scale.z = 2.0f;

	worldMatrix_ = transform_.SetMatrix();

}

void Ocean::Term() {
}
