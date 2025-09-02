#include "RectTransformComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RectTransformComponent::ShowComponentInspector() {
	transform_.SetImGuiCommand();
	UpdateMatrix();
}

void RectTransformComponent::CreateBuffer() {
	if (buffer_ != nullptr) {
		return;
	}

	buffer_ = std::make_unique<DimensionBuffer<Matrix4x4>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0] = Matrix4x4::Identity();
}

const D3D12_GPU_VIRTUAL_ADDRESS& RectTransformComponent::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

void RectTransformComponent::UpdateMatrix() {
	mat_ = transform_.ToMatrixPivot();

	if (auto parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<RectTransformComponent>()) {
			mat_ *= component->mat_;
		}
	}

	TransferGPU();
}

bool RectTransformComponent::HasParent() const {
	if (auto parent = BaseComponent::GetBehaviour()->GetParent()) {
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

json RectTransformComponent::PerseToJson() const {
	json component = json::object();
	component["transform"] = transform_.PerseToJson();

	return component;
}

void RectTransformComponent::InputJson(const json& data) {
	transform_.InputJson(data.at("transform"));
}

void RectTransformComponent::TransferGPU() {
	if (buffer_ != nullptr) {
		(*buffer_)[0] = mat_;
	}
}
