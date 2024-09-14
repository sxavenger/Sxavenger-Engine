#include "Transform.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTransformBuffer::Init() {
	// GPUBufferの生成
	buffer_ = std::make_unique<BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), 1);
	(*buffer_)[0].Init(); //!< 単位行列で初期化
}

void BaseTransformBuffer::Transfer() {

	if (parent_ != nullptr) {
		mat_ *= parent_->GetWorldMatrix();
	}

	if (buffer_ != nullptr) {
		(*buffer_)[0].worldMatrix           = mat_;
		(*buffer_)[0].worldInverceTranspose = mat_.Inverse().Transpose();
	}
}

const D3D12_GPU_VIRTUAL_ADDRESS BaseTransformBuffer::GetGPUVirtualAddress() const {
	Assert(buffer_ != nullptr, "transform buffer not initalized.");
	return buffer_->GetGPUVirtualAddress();
}

const Vector3f BaseTransformBuffer::GetWorldPosition() const {
	Vector3f result = {};

	result.x = mat_.m[3][0];
	result.y = mat_.m[3][1];
	result.z = mat_.m[3][2];

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransformBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EulerTransformBuffer::Init() {
	BaseTransformBuffer::Init();
}

void EulerTransformBuffer::Term() {
}

void EulerTransformBuffer::UpdateMatrix() {
	mat_ = transform.ToMatrix();
	Transfer();
}

void EulerTransformBuffer::SetImGuiCommand() {
	transform.SetImGuiCommand();

	UpdateMatrix();
}

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransformBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void QuaternionTransformBuffer::Init() {
	BaseTransformBuffer::Init();
	transform.rotate = Quaternion::Identity();
}

void QuaternionTransformBuffer::Term() {
}

void QuaternionTransformBuffer::UpdateMatrix() {
	mat_ = transform.ToMatrix();
	Transfer();
}

void QuaternionTransformBuffer::SetImGuiCommand() {
	transform.SetImGuiCommand();
	UpdateMatrix();
}

