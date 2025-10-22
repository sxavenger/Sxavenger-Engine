#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief スキニング用のComputePipelineクラス.
class SkinningComputePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinningComputePipeline()  = default;
	~SkinningComputePipeline() = default;

	void Init();

	void Term();

	void SetPipeline(const DirectXQueueContext* context);

	void Dispatch(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ReflectionComputePipelineState> state_;

	static const uint32_t kNumthreads_ = 32;

};




