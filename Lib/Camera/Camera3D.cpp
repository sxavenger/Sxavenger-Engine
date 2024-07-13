#include "Camera3D.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Lib
#include "Environment.h" // HACK:
#include "MyEngine.h"
#include "PrimitiveDrawer.h"

// imgui
#include <externals/imgui/imgui.h>

// Adapter
#include <Json.h>

#include "Console.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3D method
////////////////////////////////////////////////////////////////////////////////////////////

Camera3D::Camera3D() { Init(); }

Camera3D::~Camera3D() { Term(); }

void Camera3D::Init() {
	SetCamera(unitVector, origin, {0.0f, 0.0f, -10.0f});
	SetProjection(0.45f, static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.1f, 100.0f);

	resource_ = std::make_unique<DxObject::BufferPtrResource<CameraForGPU>>(MyEngine::GetDevicesObj(), 1);
	resource_->SetPtr(0, &cameraForGPU_);
}

void Camera3D::Term() {
	resource_.reset();
}

void Camera3D::DrawFrustum(const Vector4f& color) {

	Vector3f frustumPoint[4];
	Matrix4x4 clipMatrix = projectionMatrix_.Inverse();
	Matrix4x4 worldMatrix = viewMatrix_.Inverse();

	frustumPoint[0] = Matrix::EulerTransform(Matrix::EulerTransform({-1.0f, -1.0f, 1.0f}, clipMatrix), worldMatrix);
	frustumPoint[1] = Matrix::EulerTransform(Matrix::EulerTransform({-1.0f, 1.0f, 1.0f}, clipMatrix), worldMatrix);
	frustumPoint[2] = Matrix::EulerTransform(Matrix::EulerTransform({1.0f, 1.0f, 1.0f}, clipMatrix), worldMatrix);
	frustumPoint[3] = Matrix::EulerTransform(Matrix::EulerTransform({1.0f, -1.0f, 1.0f}, clipMatrix), worldMatrix);

	// drawerの取得
	auto drawer = PrimitiveDrawer::GetInstance();

	for (int i = 0; i < 4; ++i) {
		drawer->DrawLine(
			frustumPoint[i], frustumPoint[(i + 1) % 4], color
		);

		drawer->DrawLine(
			frustumPoint[i], camera_.translate, color
		);
	}

	drawer->DrawAll3D();

}

void Camera3D::SetCamera(const Vector3f& scale, const Vector3f& rotate, const Vector3f& transform) {
	camera_ = { scale, rotate, transform };

	Matrix4x4 cameraMatrix = Matrix::MakeAffine(camera_.scale, camera_.rotate, camera_.translate);
	viewMatrix_ = cameraMatrix.Inverse();

	cameraForGPU_.viewMatrix = viewMatrix_;
	cameraForGPU_.position = { camera_.translate.x, camera_.translate.y, camera_.translate.z, 1.0f };
}

void Camera3D::SetProjection(float fovY, float aspectRatio, float nearClip, float farClip) {
	projectionMatrix_ = Matrix::MakePerspectiveFov(fovY, aspectRatio, nearClip, farClip);

	cameraForGPU_.projMatrix = projectionMatrix_;
}

void Camera3D::SetAttributeImGui() {
	ImGui::DragFloat3("scale", &camera_.scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotate.x, 0.01f);
	ImGui::DragFloat3("translate", &camera_.translate.x, 0.01f);

	RecalculateCamera();
}

void Camera3D::RecalculateCamera() {
	Matrix4x4 cameraMatrix = Matrix::MakeAffine(camera_.scale, camera_.rotate, camera_.translate);
	viewMatrix_ = cameraMatrix.Inverse();

	cameraForGPU_.position = { camera_.translate.x, camera_.translate.y, camera_.translate.z, 1.0f };
	cameraForGPU_.viewMatrix = viewMatrix_;
}
