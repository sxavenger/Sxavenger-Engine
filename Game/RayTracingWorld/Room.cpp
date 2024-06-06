#include "Room.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Room class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Room::Init() {

	// transform
	transform_.translate = { 30.0f, 0.0f, -30.0f };

	worldMatrix_ = transform_.SetMatrix();

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "room.obj");

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		model_->GetMeshData(0).vertexResource.get(), model_->GetMeshData(0).indexResource.get(),
		L"room"
	);

}

void Room::Term() {
}
