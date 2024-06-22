#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxrObject
#include <DxrAccelerationStructure.h>

// DxObject
#include <DxBufferResource.h>

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

	RayTracingObject() { Init(); }

	void Init();

	DxrObject::BottomLevelAS* GetBlas() const { return blas_.get(); };

	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const { return matrixBuffer_->GetGPUVirtualAddress(); }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxrObject::BottomLevelAS> blas_;

	Transform transform_;
	Matrix4x4 worldMatrix_ = Matrix4x4::MakeIdentity();

	std::unique_ptr<DxObject::BufferPtrResource<Matrix4x4>> matrixBuffer_;

};