#include "CameraComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void CameraComponent::Camera::Init() {
	view    = Matrix4x4::Identity();
	world   = Matrix4x4::Identity();
	proj    = Matrix4x4::Identity();
	projInv = Matrix4x4::Identity();
}

void CameraComponent::Camera::TransferView(const Matrix4x4& _world) {
	world = _world;
	view  = world.Inverse();
}

void CameraComponent::Camera::TransferProj(const Matrix4x4& _proj) {
	proj    = _proj;
	projInv = proj.Inverse();
}

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CameraComponent::CreateBuffer() {
	buffer_ = std::make_unique<DimensionBuffer<Camera>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0].Init();
}

void CameraComponent::UpdateViewMatrix() {
	TransformComponent::UpdateMatrix();
	(*buffer_)[0].TransferView(GetMatrix());
}

const D3D12_GPU_VIRTUAL_ADDRESS& CameraComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "camera buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}
