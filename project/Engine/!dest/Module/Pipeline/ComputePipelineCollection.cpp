#include "ComputePipelineCollection.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Config/SxavengerDirectory.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComputePipelineCollection::Init() {
	CreateTransition();
	CreateVisual();
	CreateXclipse();
}

void ComputePipelineCollection::Term() {
}

void ComputePipelineCollection::SetPipeline(ComputePipelineType type, const DirectXThreadContext* context) {
	pipelines_[type]->ReloadAndSetPipeline(context);
}

void ComputePipelineCollection::BindComputeBuffer(ComputePipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[type]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void ComputePipelineCollection::Dispatch(const DirectXThreadContext* context, const Vector2ui& size) {
	context->GetCommandList()->Dispatch(RoundUp(size.x, numthreads_.x), RoundUp(size.y, numthreads_.y), 1);
}

void ComputePipelineCollection::CreateTransition() {

	pipelines_[kTransition_SampleLighting] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[kTransition_SampleLighting]->CreateAsset(kPackagesShaderDirectory / "sxavenger/transition/transitionSampleLighting.cs.hlsl");
	pipelines_[kTransition_SampleLighting]->RegisterBlob();
	pipelines_[kTransition_SampleLighting]->ReflectionPipeline(SxavengerSystem::GetDxDevice());

}

void ComputePipelineCollection::CreateVisual() {

	pipelines_[kVisual_DoF] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[kVisual_DoF]->CreateAsset(kPackagesShaderDirectory / "sxavenger/visual/visualDoF.cs.hlsl");
	pipelines_[kVisual_DoF]->RegisterBlob();
	pipelines_[kVisual_DoF]->ReflectionPipeline(SxavengerSystem::GetDxDevice());

}

void ComputePipelineCollection::CreateXclipse() {
	pipelines_[kXclipse_Atmosphere] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[kXclipse_Atmosphere]->CreateAsset(kPackagesShaderDirectory / "sxavenger/xclipse/xclipseAtmosphere.cs.hlsl");
	pipelines_[kXclipse_Atmosphere]->RegisterBlob();
	pipelines_[kXclipse_Atmosphere]->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	pipelines_[kXclipse_SSAO] = std::make_unique<CustomReflectionComputePipeline>();
	pipelines_[kXclipse_SSAO]->CreateAsset(kPackagesShaderDirectory / "sxavenger/xclipse/xclipseSSAO.cs.hlsl");
	pipelines_[kXclipse_SSAO]->RegisterBlob();
	pipelines_[kXclipse_SSAO]->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}
