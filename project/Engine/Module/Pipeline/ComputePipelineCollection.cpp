#include "ComputePipelineCollection.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComputePipelineCollection::Init() {
	CreateTransition();
	CreateVisual();
}

void ComputePipelineCollection::Term() {
}

void ComputePipelineCollection::SetPipeline(ComputePipelineType type, const DirectXThreadContext* context) {
	pipelines_[type]->ReloadAndSetPipeline(context->GetDxCommand());
}

void ComputePipelineCollection::BindComputeBuffer(ComputePipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[type]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void ComputePipelineCollection::Dispatch(const DirectXThreadContext* context, const Vector2ui& size) {
	context->GetCommandList()->Dispatch(RoundUp(size.x, numthreads_.x), RoundUp(size.y, numthreads_.y), 1);
}

void ComputePipelineCollection::CreateTransition() {

	pipelines_[kTransition_SampleLighting] = std::make_unique<ReflectionComputePipelineState>();
	pipelines_[kTransition_SampleLighting]->CreateBlob("sxavenger/transition/transitionSampleLighting.cs.hlsl");
	pipelines_[kTransition_SampleLighting]->ReflectionPipeline(SxavengerSystem::GetDxDevice());

}

void ComputePipelineCollection::CreateVisual() {

	pipelines_[kVisual_DoF] = std::make_unique<ReflectionComputePipelineState>();
	pipelines_[kVisual_DoF]->CreateBlob("sxavenger/visual/visualDoF.cs.hlsl");
	pipelines_[kVisual_DoF]->ReflectionPipeline(SxavengerSystem::GetDxDevice());

}
