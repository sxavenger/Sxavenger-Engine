#include "Camera3D.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Lib
#include <Lib/Environment.h>

// engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3D class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Camera3D::Init() {

	// bufferの設定
	buffer_ = std::make_unique<BufferResource<CameraForGPU>>(Sxavenger::GetDevicesObj(), 1);

	// cameraの設定
	SetTransform(kUnit3, kOrigin3, {0.0f, 0.0f, -10.0f});
	SetProjection(0.45f, static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y), 0.1f, 100.0f);

}

void Camera3D::Term() {
}

void Camera3D::SetTransform(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) {

	transform_.transform.scale     = scale;
	transform_.transform.rotate    = rotate;
	transform_.transform.translate = translate;

	UpdateTranslate();
}

void Camera3D::UpdateTranslate() {
	transform_.UpdateMatrix();
	CalculateView();
}

void Camera3D::SetProjection(float fovY, float aspectRatio, float nearClip, float farClip) {
	(*buffer_)[0].projMatrix        = Matrix::MakePerspectiveFov(fovY, aspectRatio, nearClip, farClip);
	(*buffer_)[0].projInverseMatrix = Matrix::MakePerspectiveFov(fovY, aspectRatio, nearClip, farClip).Inverse();
}

void Camera3D::DrawFrustum(const Color4f& color, float length) const {

	Vector3f frustumPoint[4] = {};
	Matrix4x4 clipMatrix  = (*buffer_)[0].projMatrix.Inverse();
	Matrix4x4 worldMatrix = (*buffer_)[0].viewMatrix.Inverse();

	frustumPoint[0] = Matrix::Transform(Matrix::Transform({ -1.0f, -1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[1] = Matrix::Transform(Matrix::Transform({ -1.0f, 1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[2] = Matrix::Transform(Matrix::Transform({ 1.0f, 1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[3] = Matrix::Transform(Matrix::Transform({ 1.0f, -1.0f, 1.0f }, clipMatrix), worldMatrix);

	// 視錐台の長さの指定がある場合, その長さに合わせる
	if (length != 0.0f) {
		for (int i = 0; i < 4; ++i) {
			Vector3f direction = frustumPoint[i] - transform_.GetWorldPosition();
			frustumPoint[i]    = transform_.GetWorldPosition() + Normalize(direction) * length;
		}
	}

	for (int i = 0; i < 4; ++i) {
		SxavengerGame::DrawLine(
			frustumPoint[i], frustumPoint[(i + 1) % 4], color
		);

		SxavengerGame::DrawLine(
			frustumPoint[i], transform_.GetWorldPosition(), color
		);
	}
}

void Camera3D::CalculateView() {
	Vector3f position = transform_.GetWorldPosition();
	(*buffer_)[0].position    = { position.x, position.y, position.z };
	(*buffer_)[0].worldMatrix = transform_.GetWorldMatrix();
	(*buffer_)[0].viewMatrix  = transform_.GetWorldMatrix().Inverse();
}

void Camera3D::SetAttributeImGui() {
	transform_.SetImGuiCommand();
	CalculateView();
}