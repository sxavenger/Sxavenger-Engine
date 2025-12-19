#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningCompute class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief スキニング計算用クラス
class SkinningCompute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void SetPipeline(const DirectXQueueContext* context);

	void Dispatch(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline_;

	static const uint32_t kNumthreads = 32;

};

SXAVENGER_ENGINE_NAMESPACE_END
