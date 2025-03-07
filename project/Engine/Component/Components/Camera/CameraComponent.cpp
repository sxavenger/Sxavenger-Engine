#include "CameraComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../MonoBehaviour.h"
#include "../Transform/TransformComponent.h"

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
// Projection structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void CameraComponent::Projection::Init() {
	sensor = { 16.0f, 9.0f };
	focal  = 20.0f;
	nearZ  = 0.01f;
	farZ   = 1024.0f;
}

Matrix4x4 CameraComponent::Projection::ToProj() const {
	float aspect = sensor.x / sensor.y;
	float fovY   = 2.0f * std::atan(sensor.y / (2.0f * focal));
	return Matrix::MakePerspectiveFov(fovY, aspect, nearZ, farZ);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CameraComponent::InspectorImGui() {
	UpdateView();

	ImGui::DragFloat2("sensor", &projection_.sensor.x, 0.01f);
	ImGui::DragFloat("focal", &projection_.focal, 0.01f);
	ImGui::DragFloat("nearZ", &projection_.nearZ, 0.01f);
	ImGui::DragFloat("farZ", &projection_.farZ, 0.01f);
	UpdateProj();
}

void CameraComponent::Init() {
	CreateBuffer();

	projection_.Init();
	UpdateProj();
}

void CameraComponent::CreateBuffer() {
	if (buffer_ != nullptr) {
		return;
	}

	buffer_ = std::make_unique<DxObject::DimensionBuffer<Camera>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0].Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& CameraComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "camera buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

void CameraComponent::UpdateView() {
	if (buffer_ == nullptr) {
		return;
	}

	// transform component から view matrix を取得
	auto transform = BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
	buffer_->At(0).TransferView(transform->GetMatrix());
}

void CameraComponent::UpdateProj() {
	if (buffer_ == nullptr) {
		return;
	}

	// projection から proj matrix を取得
	buffer_->At(0).TransferProj(projection_.ToProj());
}

const CameraComponent::Camera& CameraComponent::GetCamera() const {
	Assert(buffer_ != nullptr, "camera buffer is not craete.");
	return (*buffer_)[0];
}
