#include "Transform2dComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform2dComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTransform2dComponent::CreateBuffer() {
	buffer_ = std::make_unique<DxObject::DimensionBuffer<Transformation2d>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0).Init();
}

void BaseTransform2dComponent::Transfer() {
	if (HasParent()) {
		mat_ *= parent_->mat_;
	}

	if (buffer_ != nullptr) {
		buffer_->At(0).Transfer(mat_);
	}
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseTransform2dComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "transform2d buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transform2dComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Transform2dComponent::UpdateMatrix() {
	mat_ = transform2d_.ToMatrix();
	BaseTransform2dComponent::Transfer();
}
