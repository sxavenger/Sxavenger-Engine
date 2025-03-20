#include "TransformComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::InspectorImGui() {
	transform_.SetImGuiCommand();
	UpdateMatrix();
}

json TransformComponent::OutputJson() const {
	json component;

	component["transform"] = transform_.OutputJson();

	return component;
}

void TransformComponent::InputJson(const json& data) {
	if (!data.contains(typeid(TransformComponent).name())) {
		return; //!< component data is not exist.
	}

	const json& component = data.at(typeid(TransformComponent).name());
	transform_.InputJson(component.at("transform"));
}

void TransformComponent::CreateBuffer() {
	if (buffer_ != nullptr) {
		return;
	}

	buffer_ = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0].Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& TransformComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

const Vector3f TransformComponent::GetPosition() const {
	return { mat_.m[3][0], mat_.m[3][1], mat_.m[3][2] };
}

void TransformComponent::UpdateMatrix() {
	mat_ = transform_.ToMatrix();

	if (auto parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			mat_ *= component->mat_;
		}
	}

	TransferGPU();
}

bool TransformComponent::HasParent() const {
	if (auto parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			return true;
		}
	}

	return false;
}

void TransformComponent::TransferGPU() {
	if (buffer_ != nullptr) {
		(*buffer_)[0].Transfer(mat_);
	}
}
