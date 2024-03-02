#include "Camera3D.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Lib
#include "Environment.h" // HACK:
#include "MyEngine.h"

// imgui
#include <externals/imgui/imgui.h>

// Adapter
#include <Json.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3D method
////////////////////////////////////////////////////////////////////////////////////////////

Camera3D::Camera3D() { Init(); }

Camera3D::Camera3D(const std::string& filePath) {
	ReadJsonCameraData(filePath);
	SetProjection(0.45f, static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.1f, 100.0f);

	resource_ = new DxObject::BufferPtrResource<Vector3f>(MyEngine::GetDevice(), 1);
	resource_->Set(0, &direction);
}

Camera3D::~Camera3D() { Term(); }

void Camera3D::Init() {
	SetCamera(unitVector, origin, {0.0f, 0.0f, -10.0f});
	SetProjection(0.45f, static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.1f, 100.0f);

	/*resource_ = new DxObject::BufferPtrResource<Vector3f>(MyEngine::GetDevice(), 1);
	resource_->Set(0, &direction);*/
}

void Camera3D::Term() {
	resource_.Release();
}

void Camera3D::SetCamera(const Vector3f& scale, const Vector3f& rotate, const Vector3f& transform) {
	camera_ = { scale, rotate, transform };

	Matrix4x4 cameraMatrix = Matrix::MakeAffine(camera_.scale, camera_.rotate, camera_.translate);
	viewMatrix_ = Matrix::Inverse(cameraMatrix);

	/*position = { camera_.translate.x, camera_.translate.y, camera_.translate.z, 1.0f };*/
	direction = Matrix::Transform({0.0f, 0.0f, 1.0f}, Matrix::MakeRotate(rotate));
}

void Camera3D::SetProjection(float fovY, float aspectRatio, float nearClip, float farClip) {
	projectionMatrix_ = Matrix::MakePerspectiveFov(fovY, aspectRatio, nearClip, farClip);
}

void Camera3D::UpdateImGui(const char* title, const char* cameraName) {
	ImGui::Begin(title);

	if (ImGui::TreeNode(cameraName)) {

		ImGui::DragFloat3("scale", &camera_.scale.x, 0.01f);
		ImGui::DragFloat3("rotate", &camera_.rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &camera_.translate.x, 0.01f);

		RecalculateCamera();

		ImGui::TreePop();
	}

	ImGui::End();
}

void Camera3D::ReadJsonCameraData(const std::string& filePath) {
	Json parameter = JsonAdapter::ReadJson(filePath);

	camera_.scale = {
		parameter["transform"]["scale"]["x"],
		parameter["transform"]["scale"]["y"],
		parameter["transform"]["scale"]["z"],
	};

	camera_.rotate = {
		parameter["transform"]["rotate"]["x"],
		parameter["transform"]["rotate"]["y"],
		parameter["transform"]["rotate"]["z"],
	};

	camera_.translate = {
		parameter["transform"]["translate"]["x"],
		parameter["transform"]["translate"]["y"],
		parameter["transform"]["translate"]["z"],
	};

	RecalculateCamera();
}

void Camera3D::RecalculateCamera() {
	Matrix4x4 cameraMatrix = Matrix::MakeAffine(camera_.scale, camera_.rotate, camera_.translate);
	viewMatrix_ = Matrix::Inverse(cameraMatrix);

	direction = Matrix::Transform({ 0.0f, 0.0f, 1.0f }, Matrix::MakeRotate(camera_.rotate));
}
