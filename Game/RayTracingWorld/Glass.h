#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// dxrObject
#include <DxrAccelerationStructure.h>

// dxObject
#include <DxBufferResource.h>
#include <DxStructuredBuffer.h>

// c++
#include <memory>
#include <vector>

// model
#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Glass class
////////////////////////////////////////////////////////////////////////////////////////////
class Glass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Glass() { Init(); }

	~Glass() { Term(); }

	void Init();

	void Term();

	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }

	uint32_t GetModelSize() const { return modelSize_; }

	DxrObject::BottomLevelAS* GetBlas(uint32_t index) const { return blasArray_[index].get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return structuredBuffer_->GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	// IA
	std::unique_ptr<Model> model_;
	std::vector<std::unique_ptr<DxrObject::BottomLevelAS>> blasArray_;

	uint32_t modelSize_;

	// world
	Transform transform_;
	Matrix4x4 worldMatrix_;

	// buffer
	std::unique_ptr<DxObject::BufferResource<Vector4f>> colorBuffers_;
	std::unique_ptr<DxObject::StructuredBuffer> structuredBuffer_;

};