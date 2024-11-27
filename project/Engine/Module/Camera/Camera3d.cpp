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
	transform_.translate = kUnit3<float>;
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
	return json();
}

void Camera3d::InputJson(const json& data) {
	data;
}


