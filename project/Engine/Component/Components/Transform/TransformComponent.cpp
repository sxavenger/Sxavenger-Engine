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

void TransformComponent::TransferGPU() {
	if (buffer_ != nullptr) {
		(*buffer_)[0].Transfer(mat_);
	}
}
