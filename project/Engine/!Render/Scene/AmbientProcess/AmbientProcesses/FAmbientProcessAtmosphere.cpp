#include "FAmbientProcessAtmosphere.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FAmbientProcessAtmosphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FAmbientProcessAtmosphere::Init() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void FAmbientProcessAtmosphere::Process(const ProcessContext& context) {
	D3D12_RESOURCE_BARRIER barrier = context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Main)->TransitionBeginUnordered();
	context.context->GetCommandList()->ResourceBarrier(1, &barrier);

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::Atmosphere, context.context);

	BindBufferDesc parameter = context.parameter;
	parameter.SetHandle("gMain",       context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleUAV());
	parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::Atmosphere, context.context, parameter);
	FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);

	barrier = context.textures->GetGBuffer(FSceneTextures::GBufferLayout::Main)->TransitionEndUnordered();
	context.context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FAmbientProcessAtmosphere::SetImGuiCommand() {
}
