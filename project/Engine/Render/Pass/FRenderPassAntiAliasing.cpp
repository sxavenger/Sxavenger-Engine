#include "FRenderPassAntiAliasing.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassAntiAliasing class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassAntiAliasing::Render(const DirectXQueueContext* context, const Config& config) {

	if (config.antiAliasing == Config::AntiAliasing::None) {
		return;
	}

	// FXAA以外が追加された場合, 分岐する

	BeginProcessAntiAliasing(context, config.buffer);

	switch (config.antiAliasing) {
		case Config::AntiAliasing::FXAA:
			ProcessFXAA(context, config.buffer);
			break;

		case Config::AntiAliasing::SMAA_1x:
			ProcessSMAA(context, config.buffer);
			break;
	}

	EndProcessAntiAliasing(context, config.buffer);

}

void FRenderPassAntiAliasing::BeginProcessAntiAliasing(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	buffer->GetProcessTextures()->BeginProcess(context, buffer->GetGBuffer(FMainGBuffer::Layout::Scene));
}

void FRenderPassAntiAliasing::EndProcessAntiAliasing(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	buffer->GetProcessTextures()->EndProcess(context, buffer->GetGBuffer(FMainGBuffer::Layout::Scene));
}

void FRenderPassAntiAliasing::ProcessFXAA(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto core = FRenderCore::GetInstance()->GetProcess();

	auto process = buffer->GetProcessTextures();
	process->NextProcess();

	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	core->SetPipeline(FRenderCoreProcess::CompositeType::FXAA, context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

	static const Vector3f kParameter = { 0.75f, 0.125f, 0.0833f };
	//!< todo: parameterを変更可能に

	desc.Set32bitConstants("Parameter", 3, &kParameter);

	core->BindComputeBuffer(FRenderCoreProcess::CompositeType::FXAA, context, desc);
	core->Dispatch(context, buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);

}

void FRenderPassAntiAliasing::ProcessSMAA(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto core = FRenderCore::GetInstance()->GetProcess();

	auto process = buffer->GetProcessTextures();

	{ //!< Edge Detection Pass
		process->NextProcess();
		process->GetCurrentTexture()->TransitionBeginUnordered(context);

		core->SetPipeline(FRenderCoreProcess::CompositeType::SMAA_EdgeDetection, context);

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());
		desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeType::SMAA_EdgeDetection, context, desc);
		core->Dispatch(context, buffer->GetSize());

		process->GetCurrentTexture()->TransitionEndUnordered(context);
	}

	{ //!< Blend Weight Calculation Pass
		process->NextProcess();
		process->GetCurrentTexture()->TransitionBeginUnordered(context);

		core->SetPipeline(FRenderCoreProcess::CompositeType::SMAA_BlendWeight, context);

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());
		desc.SetHandle("gEdge",          process->GetPrevTexture()->GetGPUHandleSRV());
		desc.SetHandle("gOutput",        process->GetCurrentTexture()->GetGPUHandleUAV());
		desc.SetHandle("gAreaTexture",   FRenderCore::GetInstance()->GetSMAAAreaTexture());
		desc.SetHandle("gSearchTexture", FRenderCore::GetInstance()->GetSMAASearchTexture());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeType::SMAA_BlendWeight, context, desc);
		core->Dispatch(context, buffer->GetSize());

		process->GetCurrentTexture()->TransitionEndUnordered(context);
	}

	{ //!< Neighborhood Blending Pass
		process->NextProcess();
		process->GetCurrentTexture()->TransitionBeginUnordered(context);

		core->SetPipeline(FRenderCoreProcess::CompositeType::SMAA_NeighborhoodBlending, context);

		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());
		desc.SetHandle("gInput",       buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleSRV());
		desc.SetHandle("gBlendWeight", process->GetPrevTexture()->GetGPUHandleSRV());
		desc.SetHandle("gOutput",      process->GetCurrentTexture()->GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeType::SMAA_NeighborhoodBlending, context, desc);
		core->Dispatch(context, buffer->GetSize());

		process->GetCurrentTexture()->TransitionEndUnordered(context);
	}
}
