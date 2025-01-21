#include "ACameraActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera structure methods
///////////////////////////////////////////////////////////////////////////////////////////

void ACameraActor::Camera::Init() {
	view    = Matrix4x4::Identity();
	world   = Matrix4x4::Identity();
	proj    = Matrix4x4::Identity();
	projInv = Matrix4x4::Identity();
}

void ACameraActor::Camera::TransferView(const Matrix4x4& _world) {
	world = _world;
	view  = world.Inverse();
}

void ACameraActor::Camera::TransferProj(const Matrix4x4& _proj) {
	proj    = _proj;
	projInv = proj.Inverse();
}

////////////////////////////////////////////////////////////////////////////////////////////
// ACameraActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ACameraActor::Init() {
	buffer_ = std::make_unique<DxObject::DimensionBuffer<Camera>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0].Init();

	UpdateProj(Matrix::MakePerspectiveFov(0.45f, static_cast<float>(kMainWindowSize.x) / static_cast<float>(kMainWindowSize.y), 0.1f, 1024.0f));

	AActor::name_ = "camera actor";
}

void ACameraActor::UpdateView() {
	TransformComponent::UpdateMatrix();
	(*buffer_)[0].TransferView(GetMatrix());
}

void ACameraActor::UpdateProj(const Matrix4x4& proj) {
	(*buffer_)[0].TransferProj(proj);
}

const D3D12_GPU_VIRTUAL_ADDRESS& ACameraActor::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "camera actor not init.");
	return buffer_->GetGPUVirtualAddress();
}
