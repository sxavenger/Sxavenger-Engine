#include "Transform2dComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform2dComponent class
////////////////////////////////////////////////////////////////////////////////////////

void BaseTransform2dComponent::CreateBuffer() {
	buffer_ = std::make_unique<DimensionBuffer<Matrix4x4>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*buffer_)[0] = mat_;
}

void BaseTransform2dComponent::Transfer() {
	if (HasParent()) {
		mat_ *= parent_->mat_;
	}

	if (buffer_ != nullptr) {
		(*buffer_)[0] = mat_;
	}
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseTransform2dComponent::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "transform2d buffer is not craete.");
	return buffer_->GetGPUVirtualAddress();
}

////////////////////////////////////////////////////////////////////////////////////////
// Transform2dComponent class
////////////////////////////////////////////////////////////////////////////////////////

void Transform2dComponent::UpdateMatrix() {
	mat_ = Matrix::MakeAffine(
			{ transform2d_.scale.x, transform2d_.scale.y, 0.0f },
			{ 0.0f, 0.0f, transform2d_.rotate },
			{ transform2d_.translate.x, transform2d_.translate.y, depth_ }
	);

	BaseTransform2dComponent::Transfer();
}

json Transform2dComponent::OutputJson() {
	return json();
}

void Transform2dComponent::InputJson(const json& data) {
	data;
}
