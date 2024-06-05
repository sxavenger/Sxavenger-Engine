#include "Ground.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Ground class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Ground::Init() {
	// IA
	model_ = std::make_unique<Model>("./Resources/model", "Ground.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMeshData(0).vertexResource.get(), model_->GetMeshData(0).indexResource.get(),
		L"ground"
	);

}

void Ground::Term() {
}
