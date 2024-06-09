#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxrObject
#include <DxrAccelerationStructure.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingObject base class
////////////////////////////////////////////////////////////////////////////////////////////
class RayTracingObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DxrObject::BottomLevelAS* GetBlas() const { return blas_.get(); };

	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxrObject::BottomLevelAS> blas_;

	Transform transform_;
	Matrix4x4 worldMatrix_ = Matrix4x4::MakeIdentity();

};