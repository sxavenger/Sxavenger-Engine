#include "FRenderPassLightingComposite.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FMainBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreTransition.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassLightingComposite class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassLightingComposite::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->HasBuffer<FLightAccumulationBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Lighting Composite]", "FLightAccumulationBuffer is required.");
		return; //!< Bufferが不適格
	}

	if (!config.buffer->EnsureBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Lighting Composite]", "MainBuffer is required.");
		return; //!< Bufferが不適格
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>(); //!< RenderCoreの確保.

	FBaseRenderPass::BeginRenderPass(context, "Lighting Composite", config);
	
	{ //!< Transition Lighting Composite Pass
		TransitionLightingCompositePass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassLightingComposite::TransitionLightingCompositePass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FLightAccumulationBuffer* lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>();
	FMainBuffer* main                           = buffer->GetBuffer<FMainBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>(); //!< RenderCoreの確保
	core->SetPipeline(FRenderCoreTransition::Transition::LightingTransition, context); //!< pipelineの設定

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionUnorderedAccess(context);

	{ //!< Transition Lighting Composite Pass

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2, &buffer->GetResolution());

		//!< Bufferの設定
		desc.SetHandle("gDirect",   lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Direct).GetGPUHandleSRV());
		desc.SetHandle("gIndirect", lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleSRV());
		desc.SetHandle("gOutput",   main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreTransition::Transition::LightingTransition, context, desc);
		core->Dispatch(context, buffer->GetResolution());
	}

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionDefaultState(context);
}
