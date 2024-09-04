#include "Transform.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Sxavenger.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTransformBuffer::Init() {
	// GPUBufferの生成
	buffer_ = std::make_unique<BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), 1);
	(*buffer_)[0].Init(); //!< 単位行列で初期化
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
	(*buffer_)[0].worldMatrix = transform.ToMatrix();

	if (parent_ != nullptr) {
		(*buffer_)[0].worldMatrix *= parent_->GetWorldMatrix();
	}

	(*buffer_)[0].worldInverceTranspose = (*buffer_)[0].worldMatrix.Inverse().Transpose();
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
}

void QuaternionTransformBuffer::Term() {
}

void QuaternionTransformBuffer::UpdateMatrix() {
	(*buffer_)[0].worldMatrix = transform.ToMatrix();

	if (parent_ != nullptr) {
		(*buffer_)[0].worldMatrix *= parent_->GetWorldMatrix();
	}

	(*buffer_)[0].worldInverceTranspose = (*buffer_)[0].worldMatrix.Inverse().Transpose();
}

