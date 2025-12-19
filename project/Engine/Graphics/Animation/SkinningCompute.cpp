#include "SkinningCompute.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningCompute class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinningCompute::Init() {
	pipeline_ = std::make_unique<ReflectionComputePipelineState>();
	pipeline_->CreateBlob(kPackagesDirectory / "shaders/render/skinning/skinning.cs.hlsl");
	pipeline_->ReflectionPipeline(System::GetDxDevice());
}

void SkinningCompute::Term() {
}

void SkinningCompute::SetPipeline(const DirectXQueueContext* context) {
	pipeline_->SetPipeline(context->GetDxCommand());
}

void SkinningCompute::Dispatch(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize) {
	pipeline_->BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline_->Dispatch(context->GetDxCommand(), { RoundUp(vertexSize, kNumthreads), 1, 1 });
}
