#include "Camera3d.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Module/SxavengerModule.h>

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
	uvTransform_.scale     = kUnit3<float>;
	uvTransform_.rotate    = Quaternion::Identity();
	uvTransform_.translate = { 0.0f, 0.0f, -16.0f };
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

void Camera3d::DrawFrustum(const Color4f& color, const std::optional<float>& length) {
	Vector3f frustumPoint[4] = {};
	Matrix4x4 clipMatrix  = (*buffer_)[0].projInverseMatrix;
	Matrix4x4 worldMatrix = (*buffer_)[0].worldMatrix;

	frustumPoint[0] = Matrix::Transform(Matrix::Transform({ -1.0f, -1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[1] = Matrix::Transform(Matrix::Transform({ -1.0f, 1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[2] = Matrix::Transform(Matrix::Transform({ 1.0f, 1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[3] = Matrix::Transform(Matrix::Transform({ 1.0f, -1.0f, 1.0f }, clipMatrix), worldMatrix);

	// 視錐台の長さの指定がある場合, その長さに合わせる
	if (length.has_value()) {
		for (int i = 0; i < 4; ++i) {
			Vector3f direction = frustumPoint[i] - GetPosition();
			frustumPoint[i]    = GetPosition() + Normalize(direction) * length.value();
		}
	}

	for (int i = 0; i < 4; ++i) {
		SxavengerModule::DrawLine(
			frustumPoint[i], frustumPoint[(i + 1) % 4], color
		);

		SxavengerModule::DrawLine(
			frustumPoint[i], GetPosition(), color
		);
	}
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


