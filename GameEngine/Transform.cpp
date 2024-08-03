#include "Transform.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTransform base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTransform::Init() {
	// GPUBufferの生成
	buffer_ = std::make_unique<BufferResource<TransformationMat>>(MyEngine::GetDevicesObj(), 1);
	(*buffer_)[0].Init();
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformEuler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformEuler::Init() {
	BaseTransform::Init();
}

void TransformEuler::Term() {
}

void TransformEuler::UpdateMatrix() {
	(*buffer_)[0].worldMatrix = Matrix::MakeAffine(scale, rotation, translation);

	if (parent_ != nullptr) {
		(*buffer_)[0].worldMatrix *= parent_->GetWorldMatrix();
	}

	(*buffer_)[0].worldInverceTranspose = (*buffer_)[0].worldMatrix.Inverse().Transpose();
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformEuler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformQuaternion::Init() {
	BaseTransform::Init();
}

void TransformQuaternion::Term() {
}

void TransformQuaternion::UpdateMatrix() {
	(*buffer_)[0].worldMatrix = Matrix::MakeAffine(scale, quaternion, translation);

	if (parent_ != nullptr) {
		(*buffer_)[0].worldMatrix *= parent_->GetWorldMatrix();
	}

	(*buffer_)[0].worldInverceTranspose = (*buffer_)[0].worldMatrix.Inverse().Transpose();
}

