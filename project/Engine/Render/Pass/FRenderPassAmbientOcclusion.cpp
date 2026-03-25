#include "FRenderPassAmbientOcclusion.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreProcess.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassAmbientOcclusion class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassAmbientOcclusion::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.ambientOcclusion == FRenderConfig::AmbientOcclusion::None) {
		return; //!< Ambient-Occlusionが無効
	}

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格.
	}

	if (!config.buffer->HasBuffer<FGBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Ambient-Occlusion]", "FGBuffer is required."); //!< Bufferがない
		return; //!< Bufferが不適格
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.

	FBaseRenderPass::BeginRenderPass(context, "Ambient-Occlusion", config);

	{ //!< Ambient-Occlusion Pass

		BeginAntiAliasingPass(context, config.buffer);

		switch (config.ambientOcclusion) {
			case FRenderConfig::AmbientOcclusion::XeGTAO:
				PassAntiAliasing_XeGTAO(context, config);
				break;
		}

		EndAntiAliasingPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);
}

void FRenderPassAmbientOcclusion::BeginAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FGBuffer* gbuffer = buffer->GetBuffer<FGBuffer>();

	//!< MaterialARMをUAVに遷移(Ambient-Occlusionの書き込み用)
	gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).TransitionUnorderedAccess(context);

}

void FRenderPassAmbientOcclusion::EndAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FGBuffer* gbuffer = buffer->GetBuffer<FGBuffer>();

	//!< MaterialARMをUAVに遷移(Ambient-Occlusionの書き込み用)
	gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).TransitionDefaultState(context);

}

void FRenderPassAmbientOcclusion::PassAntiAliasing_XeGTAO(const DirectXQueueContext* context, const FRenderConfig& config) {

	FProcessBuffer* process            = config.buffer->GetProcess();
	FDepthStencilTexture* depthStencil = config.buffer->GetDepthStencil();
	FGBuffer* gbuffer                  = config.buffer->GetBuffer<FGBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",  depthStencil->GetGPUHandleSRV());
	desc.SetHandle("gNormal", gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());

	{ //!< Prefilter Depth Pass

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);
		//!< 制約: Mip5まで使用.

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::XeGTAO_PrefilterDepth, context);

		//!< Bufferの設定
		desc.SetHandle("gDepthWorkingMip0", process->GetCurrentTexture().GetGPUHandleUAV(0));
		desc.SetHandle("gDepthWorkingMip1", process->GetCurrentTexture().GetGPUHandleUAV(1));
		desc.SetHandle("gDepthWorkingMip2", process->GetCurrentTexture().GetGPUHandleUAV(2));
		desc.SetHandle("gDepthWorkingMip3", process->GetCurrentTexture().GetGPUHandleUAV(3));
		desc.SetHandle("gDepthWorkingMip4", process->GetCurrentTexture().GetGPUHandleUAV(4));

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::XeGTAO_PrefilterDepth, context, desc);
		context->GetCommandList()->Dispatch(
			(config.buffer->GetResolution().x + 16 - 1) / 16,
			(config.buffer->GetResolution().y + 16 - 1) / 16,
			1
		); //!< XeGTAOのnumthreads制約でそろえる.
		
		process->GetCurrentTexture().TransitionDefaultState(context);
	}

	{ //!< Main Pass

		process->Next(2);

		process->GetPreviousTexture(1).TransitionUnorderedAccess(context); //!< EdgeWorking
		process->GetCurrentTexture().TransitionUnorderedAccess(context);   //!< AOWorking

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::XeGTAO_Main, context);

		//!< Bufferの設定
		desc.SetHandle("gDepthWorking", process->GetPreviousTexture(2).GetGPUHandleSRV());
		desc.SetHandle("gEdgeWorking",  process->GetPreviousTexture(1).GetGPUHandleUAV());
		desc.SetHandle("gAOWorking",    process->GetCurrentTexture().GetGPUHandleUAV());

		//!< Randomの設定
		uint32_t random = Random::UniformDistribution<uint32_t>(0, 64);
		desc.Set32bitConstants("RandomConstant", 1, &random);

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::XeGTAO_Main, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		process->GetPreviousTexture(1).TransitionDefaultState(context);
		process->GetCurrentTexture().TransitionDefaultState(context);
	}

	uint8_t edgeIndex = 1; //!< EdgeWorkingのIndexを保存

	{ //!< Denoise First Pass

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		edgeIndex++;

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::XeGTAO_DenoiseFirst, context);

		//!< Bufferの設定
		desc.SetHandle("gEdgeWorking",     process->GetPreviousTexture(edgeIndex).GetGPUHandleSRV());
		desc.SetHandle("gAOWorking",       process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gAOWorkingOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::XeGTAO_DenoiseFirst, context, desc);
		core->Dispatch(context, { config.buffer->GetResolution().x / 2, config.buffer->GetResolution().y }); //!< XeGTAOのnumthreads制約でそろえる.

		process->GetCurrentTexture().TransitionDefaultState(context);

	}

	{ //!< Denoise Last Pass

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		edgeIndex++;

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::XeGTAO_DenoiseLast, context);

		//!< Bufferの設定
		desc.SetHandle("gEdgeWorking",     process->GetPreviousTexture(edgeIndex).GetGPUHandleSRV());
		desc.SetHandle("gAOWorking",       process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gAOWorkingOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::XeGTAO_DenoiseLast, context, desc);
		core->Dispatch(context, { config.buffer->GetResolution().x / 2, config.buffer->GetResolution().y }); //!< XeGTAOのnumthreads制約でそろえる.

		process->GetCurrentTexture().TransitionDefaultState(context);
	}

	{ //!< Resolve Pass

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::XeGTAO_Resolve, context);

		//!< Bufferの設定
		desc.SetHandle("gAOWorking",   process->GetCurrentTexture().GetGPUHandleSRV());
		desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::XeGTAO_Resolve, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

	}

}
