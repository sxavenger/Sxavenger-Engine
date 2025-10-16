#include "CameraComponent.h"
_DXOBJECT_USING

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

	component["sensor"] = JsonSerializeFormatter<Vector2f>::Serialize(projection_.sensor);
	component["focal"]  = JsonSerializeFormatter<float>::Serialize(projection_.focal);
	component["nearZ"]  = JsonSerializeFormatter<float>::Serialize(projection_.nearZ);
	component["farZ"]   = JsonSerializeFormatter<float>::Serialize(projection_.farZ);

	component["tag"] = magic_enum::enum_name(GetTag());

	return component;
}

void CameraComponent::InputJson(const json& data) {
	projection_.sensor = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("sensor"));
	projection_.focal  = JsonSerializeFormatter<float>::Deserialize(data.at("focal"));
	projection_.nearZ  = JsonSerializeFormatter<float>::Deserialize(data.at("nearZ"));
	projection_.farZ   = JsonSerializeFormatter<float>::Deserialize(data.at("farZ"));
	UpdateProj();

	tag_ = magic_enum::enum_cast<Tag>(JsonSerializeFormatter<std::string>::Deserialize(data.at("tag"))).value();
}

CameraComponent::CameraComponent(MonoBehaviour* behaviour)
	: BaseComponent(behaviour) {

	CreateBuffer();

	projection_.Init();
	UpdateProj();
}

void CameraComponent::ShowComponentInspector() {

	if (ImGui::BeginCombo("tag", magic_enum::enum_name(GetTag()).data())) {
		for (const auto& [value, name] : magic_enum::enum_entries<Tag>()) {
			if (ImGui::Selectable(name.data(), GetTag() == value)) {
				SetTag(value);
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::TreeNodeEx("projection", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		ImGui::DragFloat2("sensor", &projection_.sensor.x, 0.01f);
		ImGui::DragFloat("focal",   &projection_.focal, 0.01f);
		ImGui::DragFloat("nearZ",   &projection_.nearZ, 0.01f);
		ImGui::DragFloat("farZ",    &projection_.farZ, 0.01f);
		UpdateProj();
	}

	PushLineFrustum();
}

const D3D12_GPU_VIRTUAL_ADDRESS& CameraComponent::GetGPUVirtualAddress() const {
	Exception::Assert(buffers_[currentIndex_] != nullptr, "camera buffer is not craete.");
	return buffers_[currentIndex_]->GetGPUVirtualAddress();
}

const D3D12_GPU_VIRTUAL_ADDRESS& CameraComponent::GetPrevGPUVirtualAddress() const {
	size_t prevIndex = (currentIndex_ + buffers_.size() - 1) % buffers_.size();
	Exception::Assert(buffers_[prevIndex] != nullptr, "camera buffer is not craete.");
	return buffers_[prevIndex]->GetGPUVirtualAddress();
}

void CameraComponent::SwapBuffer() {
	currentIndex_ = (currentIndex_ + 1) % buffers_.size();
}

void CameraComponent::UpdateView() {
	if (buffers_[currentIndex_] == nullptr) {
		return;
	}

	// transform component から view matrix を取得
	auto transform = BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
	buffers_[currentIndex_]->At().TransferView(transform->GetMatrix());
}

void CameraComponent::UpdateProj() {
	if (buffers_[currentIndex_] == nullptr) {
		return;
	}

	// projection から proj matrix を取得
	buffers_[currentIndex_]->At().TransferProj(projection_.ToProj());

	// projection から near, far の距離を取得
	buffers_[currentIndex_]->At().nearZ = projection_.nearZ;
	buffers_[currentIndex_]->At().farZ  = projection_.farZ;
}

const CameraComponent::Camera& CameraComponent::GetCamera() const {
	Exception::Assert(buffers_[currentIndex_] != nullptr, "camera buffer is not craete.");
	return buffers_[currentIndex_]->At();
}

Vector3f CameraComponent::GetPosition() const {
	Exception::Assert(buffers_[currentIndex_] != nullptr, "camera buffer is not craete.");
	return Matrix4x4::GetTranslation(buffers_[currentIndex_]->At().world);
}

Vector3f CameraComponent::CalculateNDCPosition(const Vector3f& point) const {
	return Matrix4x4::Transform(point, GetCamera().view * GetCamera().proj);
}

Vector3f CameraComponent::CalculateWorldPosition(const Vector3f& ndc) const {
	return Matrix4x4::Transform(ndc, GetCamera().projInv * GetCamera().world);
}

void CameraComponent::CreateBuffer() {
	for (size_t i = 0; i < buffers_.size(); ++i) {
		buffers_[i] = std::make_unique<ConstantBuffer<Camera>>();
		buffers_[i]->Create(SxavengerSystem::GetDxDevice());
		buffers_[i]->At().Init();
	}
}


void CameraComponent::PushLineFrustum() {
	Vector3f frustumPoint[8] = {};
	const Matrix4x4& clipMatrix  = buffers_[currentIndex_]->At().projInv;
	const Matrix4x4& worldMatrix = buffers_[currentIndex_]->At().world;

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

		SxavengerContent::PushLine(frustumPoint[nearIndex], frustumPoint[nearNext], color, 0.6f);
		SxavengerContent::PushLine(frustumPoint[farIndex],  frustumPoint[farNext],  color, 0.6f);
		SxavengerContent::PushLine(frustumPoint[nearIndex], frustumPoint[farIndex], color, 0.6f);
	}
}
