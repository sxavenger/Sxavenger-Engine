#include "Camera3d.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3d class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::Create() {
	buffer_ = std::make_unique<DimensionBuffer<Camera3dForGPU>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	Reset();
}

void Camera3d::Term() {
}

void Camera3d::Reset() {
	transform_.scale     = kUnit3<float>;
	transform_.rotate    = Quaternion::Identity();
	transform_.translate = { 0.0f, 0.0f, -16.0f };
	UpdateMatrix();

	SetProjection(0.45f, static_cast<float>(kMainWindowSize.x) / static_cast<float>(kMainWindowSize.y), 0.1f, 1024.0f);
}

void Camera3d::UpdateMatrix() {
	TransformComponent::UpdateMatrix();
	(*buffer_)[0].worldMatrix = mat_;
	(*buffer_)[0].viewMatrix  = (*buffer_)[0].worldMatrix.Inverse();
	(*buffer_)[0].position    = GetPosition();
}

void Camera3d::SetProjection(float fovY, float aspectRatio, float nearClip, float farClip) {
	(*buffer_)[0].projMatrix        = Matrix::MakePerspectiveFov(fovY, aspectRatio, nearClip, farClip);
	(*buffer_)[0].projInverseMatrix = (*buffer_)[0].projMatrix.Inverse();
}

json Camera3d::OutputJson() {
	json root = json::object();
	root["TransformComponent"] = TransformComponent::OutputJson();
	return root;
}

void Camera3d::InputJson(const json& data) {
	TransformComponent::InputJson(data["TransformComponent"]);
}

const D3D12_GPU_VIRTUAL_ADDRESS& Camera3d::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "camera buffer is not create.");
	return buffer_->GetGPUVirtualAddress();
}


