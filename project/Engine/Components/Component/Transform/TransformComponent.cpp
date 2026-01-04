#include "TransformComponent.h"
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
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

TransformComponent::TransformComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {
	buffer_ = std::make_unique<ConstantBuffer<TransformationMatrix>>();
	buffer_->Create(System::GetDxDevice());
	buffer_->At().Init();
}

void TransformComponent::ShowComponentInspector() {
	transform_.SetImGuiCommand();
	UpdateMatrix();
}

const D3D12_GPU_VIRTUAL_ADDRESS& TransformComponent::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

const Vector3f TransformComponent::GetPosition() const {
	return Matrix4x4::GetTranslation(mat_);
}

const Vector3f TransformComponent::GetDirection() const {
	return transform_.GetForward();
}

const TransformationMatrix& TransformComponent::GetTransformationMatrix() const {
	StreamLogger::AssertA(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->At();
}

void TransformComponent::UpdateMatrix() {
	mat_ = transform_.ToMatrix();

	if (const BehaviourAddress& parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			mat_ *= component->mat_;
		}
	}

	TransferGPU();
}

bool TransformComponent::HasParent() const {
	if (const BehaviourAddress& parent = BaseComponent::GetBehaviour()->GetParent()) {
		if (auto component = parent->GetComponent<TransformComponent>()) {
			return true;
		}
	}

	return false;
}

void TransformComponent::TransferGPU() {
	if (buffer_ != nullptr) {
		buffer_->At().Transfer(mat_);
	}
}

json TransformComponent::PerseToJson() const {
	json component = json::object();
	component["transform"] = transform_.PerseToJson();

	return component;
}

void TransformComponent::InputJson(const json& data) {
	transform_.InputJson(data.at("transform"));
}
