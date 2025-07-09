#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class SkinningComputePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinningComputePipeline()  = default;
	~SkinningComputePipeline() = default;

	void Init();

	void Term();

	void SetPipeline(const DirectXThreadContext* context);

	void Dispatch(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ReflectionComputePipelineState> state_;

	static const uint32_t kNumthreads_ = 32;

};




