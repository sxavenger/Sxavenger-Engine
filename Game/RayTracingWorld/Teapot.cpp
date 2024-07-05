#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {
	// IA
	model_ = std::make_unique<Model>("./Resources/model", "teapot.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get(),
		L"teapot"
	);

	// Transform
	transform_.scale     *= 2.0f;
	transform_.translate = { 0.0f, 1.6f, 8.0f };

	worldMatrix_ = transform_.GetMatrix();

	SetThisAttribute("teapot");

}

void Teapot::Term() {
}

void Teapot::SetAttributeImGui() {

	ImGui::DragFloat3("scale",     &transform_.scale.x, 0.01f);
	ImGui::DragFloat3("rotate",    &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.01f);

	worldMatrix_ = transform_.GetMatrix();

}
