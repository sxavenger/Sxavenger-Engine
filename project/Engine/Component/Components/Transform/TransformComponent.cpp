#include "TransformComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::ShowComponentInspector() {
	transform_.SetImGuiCommand();
	UpdateMatrix();
}

void TransformComponent::CreateBuffer() {
	if (buffer_ != nullptr) {
		return;
	}

	buffer_ = std::make_unique<ConstantBuffer<TransformationMatrix>>();
	buffer_->Create(SxavengerSystem::GetDxDevice());
	buffer_->At().Init();
}

const D3D12_GPU_VIRTUAL_ADDRESS& TransformComponent::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

const Vector3f TransformComponent::GetPosition() const {
	return Matrix4x4::GetTranslation(mat_);
}

const Vector3f TransformComponent::GetDirection() const {
	return transform_.GetForward();
}

const TransformationMatrix& TransformComponent::GetTransformationMatrix() const {
	Exception::Assert(buffer_ != nullptr, "transform buffer is not craete.");
	return buffer_->At();
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
