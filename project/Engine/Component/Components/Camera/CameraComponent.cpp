#include "CameraComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

//* external
#include <magic_enum.hpp>

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
	return Matrix4x4::PerspectiveFov(fovY, aspect, nearZ, farZ);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

json CameraComponent::PerseToJson() const {
	json component = json::object();

	component["sensor"] = GeometryJsonSerializer::ToJson(projection_.sensor);
	component["focal"]  = projection_.focal;
	component["nearZ"]  = projection_.nearZ;
	component["farZ"]   = projection_.farZ;

	return component;
}

void CameraComponent::InputJson(const json& data) {
	projection_.sensor = GeometryJsonSerializer::JsonToVector2f(data.at("sensor"));
	projection_.focal  = data.at("focal");
	projection_.nearZ  = data.at("nearZ");
	projection_.farZ   = data.at("farZ");
}

void CameraComponent::ShowComponentInspector() {

	ImGui::Text("tag");
	ImGui::Separator();

	if (ImGui::BeginCombo("## tag", magic_enum::enum_name(GetTag()).data())) {
		for (const auto& [value, name] : magic_enum::enum_entries<Tag>()) {
			if (ImGui::Selectable(name.data(), GetTag() == value)) {
				SetTag(value);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Text("projection");
	ImGui::Separator();

	ImGui::DragFloat2("sensor", &projection_.sensor.x, 0.01f);
	ImGui::DragFloat("focal", &projection_.focal, 0.01f);
	ImGui::DragFloat("nearZ", &projection_.nearZ, 0.01f);
	ImGui::DragFloat("farZ", &projection_.farZ, 0.01f);
	UpdateProj();

	PushLineFrustum();
}

void CameraComponent::Init() {
	CreateBuffer();

	projection_.Init();
	UpdateProj();
}

const D3D12_GPU_VIRTUAL_ADDRESS& CameraComponent::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "camera buffer is not craete.");
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

	// projection から near, far の距離を取得
	buffer_->At(0).nearZ = projection_.nearZ;
	buffer_->At(0).farZ  = projection_.farZ;
}

const CameraComponent::Camera& CameraComponent::GetCamera() const {
	Exception::Assert(buffer_ != nullptr, "camera buffer is not craete.");
	return (*buffer_)[0];
}

Vector3f CameraComponent::CalculateNDCPosition(const Vector3f& point) const {
	return  Matrix4x4::Transform(point, GetCamera().view * GetCamera().proj);
}

void CameraComponent::CreateBuffer() {
	if (buffer_ != nullptr) {
		return;
	}

	buffer_ = std::make_unique<DxObject::DimensionBuffer<Camera>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0].Init();
}


void CameraComponent::PushLineFrustum() {
	Vector3f frustumPoint[8] = {};
	const Matrix4x4& clipMatrix  = (*buffer_)[0].projInv;
	const Matrix4x4& worldMatrix = (*buffer_)[0].world;

	// far
	frustumPoint[0] = Matrix4x4::Transform(Matrix4x4::Transform({ -1.0f, -1.0f, 1.0f }, clipMatrix), worldMatrix);
	frustumPoint[1] = Matrix4x4::Transform(Matrix4x4::Transform({ -1.0f, 1.0f, 1.0f },  clipMatrix), worldMatrix);
	frustumPoint[2] = Matrix4x4::Transform(Matrix4x4::Transform({ 1.0f, 1.0f, 1.0f },   clipMatrix), worldMatrix);
	frustumPoint[3] = Matrix4x4::Transform(Matrix4x4::Transform({ 1.0f, -1.0f, 1.0f },  clipMatrix), worldMatrix);

	// near
	frustumPoint[4] = Matrix4x4::Transform(Matrix4x4::Transform({ -1.0f, -1.0f, 0.0f }, clipMatrix), worldMatrix);
	frustumPoint[5] = Matrix4x4::Transform(Matrix4x4::Transform({ -1.0f, 1.0f, 0.0f },  clipMatrix), worldMatrix);
	frustumPoint[6] = Matrix4x4::Transform(Matrix4x4::Transform({ 1.0f, 1.0f, 0.0f },   clipMatrix), worldMatrix);
	frustumPoint[7] = Matrix4x4::Transform(Matrix4x4::Transform({ 1.0f, -1.0f, 0.0f },  clipMatrix), worldMatrix);

	static const Color4f color = Color4f::Convert(0xFAFA00FF);

	for (uint8_t i = 0; i < 4; ++i) {

		uint8_t farIndex = i;
		uint8_t farNext  = (i + 1) % 4;

		uint8_t nearIndex = farIndex + 4;
		uint8_t nearNext  = farNext + 4;

		SxavengerContent::PushLine(frustumPoint[nearIndex], frustumPoint[nearNext], color);
		SxavengerContent::PushLine(frustumPoint[farIndex],  frustumPoint[farNext],  color);
		SxavengerContent::PushLine(frustumPoint[nearIndex], frustumPoint[farIndex],  color);
	}
}
