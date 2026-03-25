#include "RectTransformComponent.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

RectTransformComponent::RectTransformComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {
	buffer_ = std::make_unique<ConstantBuffer<Matrix4x4>>();
	buffer_->Create(System::GetDxDevice());
	buffer_->At() = Matrix4x4::Identity();
}

void RectTransformComponent::ShowComponentInspector() {
	SxGui::DragVectorN<float, 2>("translate", &transform_.translate.x, 0.1f);
	ImGui::SliderAngle("rotate", &transform_.rotate);
	SxGui::DragVectorN<float, 2>("scale", &transform_.scale.x, 0.1f);

	ImGui::Separator();

	SxGui::DragVectorN<float, 2>("pivot", &transform_.pivot.x, 0.01f, 0.0f, 1.0f, "%.2f");
	SxImGui::DragFloat("priority", &transform_.priority, 0.01f, 0.0f, 1.0f);

	UpdateMatrix();
}

const D3D12_GPU_VIRTUAL_ADDRESS RectTransformComponent::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(buffer_ != nullptr, "transform buffer is not create.");
	return buffer_->GetGPUVirtualAddress();
}

void RectTransformComponent::UpdateMatrix() {
	mat_ = transform_.ToMatrixPivot();

	if (const BehaviourAddress& parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<RectTransformComponent>()) {
			mat_ *= component->mat_;
		}
	}

	TransferGPU();
}

bool RectTransformComponent::HasParent() const {
	if (const BehaviourAddress& parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<RectTransformComponent>()) {
			return true;
		}
	}

	return false;
}

const Vector2f RectTransformComponent::GetPosition() const {
	Vector3f pos = Matrix4x4::GetTranslation(mat_);
	return { pos.x, pos.y };
}

json RectTransformComponent::ParseToJson() const {
	json component = json::object();
	component["transform"] = transform_.ParseToJson();

	return component;
}

void RectTransformComponent::InputJson(const json& data) {
	transform_.InputJson(data.at("transform"));
}

void RectTransformComponent::TransferGPU() {
	if (buffer_ != nullptr) {
		buffer_->At() = mat_;
	}
}
