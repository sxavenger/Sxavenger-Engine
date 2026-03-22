#include "FRenderPassAntiAliasing.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FMainBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreProcess.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassAntiAliasing class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassAntiAliasing::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.antiAliasing == FRenderConfig::AntiAliasing::None) {
		return; //!< anti-aliasingが無効
	}

	if (!config.buffer->HasBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[RenderPass - Anti-Aliasing]", "FMainBuffer is required."); 
		return; //!< bufferが不適格
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保

	FBaseRenderPass::BeginRenderPass(context, "Anti-Aliasing", config);

	{ //!< Anti-Aliasing Pass

		BeginAntiAliasingPass(context, config.buffer);

		switch (config.antiAliasing) {
			case FRenderConfig::AntiAliasing::FXAA:
				PassAntiAliasingFXAA(context, config);
				break;

			case FRenderConfig::AntiAliasing::SMAA_1x:
				PassAntiAliasingSMAA(context, config);
				break;
		}

		EndAntiAliasingPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);
}

void FRenderPassAntiAliasing::BeginAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main       = buffer->GetBuffer<FMainBuffer>();
	FProcessBuffer* process = buffer->GetProcess();

	process->Import(context, &main->GetBuffer(FMainBuffer::Layout::Scene));
	//!< Scene BufferをProcess処理用に使用状態にする.
}

void FRenderPassAntiAliasing::EndAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main       = buffer->GetBuffer<FMainBuffer>();
	FProcessBuffer* process = buffer->GetProcess();

	process->Export(context, &main->GetBuffer(FMainBuffer::Layout::Scene));
	//!< Process処理が完了したScene BufferをScene Bufferに戻す.
}

void FRenderPassAntiAliasing::PassAntiAliasingFXAA(const DirectXQueueContext* context, const FRenderConfig& config) {

	FProcessBuffer* process = config.buffer->GetProcess(); //!< Process Bufferの確保

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>();

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
 
	{ //!< FXAA処理

		//!< Process Bufferの準備
		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::FXAA, context);

		//!< Bufferの設定
		desc.SetHandle("gInput",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		//!< FXAAのparameterの設定 (x: subpixel, y: edge threshold, z: edge threshold min)
		static const Vector3f kParameter = { 0.75f, 0.125f, 0.0833f }; // TODO: parameterを変更可能に
		desc.Set32bitConstants("Parameter", 3, &kParameter);

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::FXAA, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void FRenderPassAntiAliasing::PassAntiAliasingSMAA(const DirectXQueueContext* context, const FRenderConfig& config) {

	FProcessBuffer* process = config.buffer->GetProcess(); //!< Process Bufferの確保

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>();

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	//!< SMAAのTextureの設定
	desc.SetHandle("gAreaTexture",   FRenderCore::GetInstance()->GetSMAAAreaTexture());
	desc.SetHandle("gSearchTexture", FRenderCore::GetInstance()->GetSMAASearchTexture());

	{ //!< Edge Detection

		//!< Process Bufferの準備
		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::SMAA_EdgeDetection, context);

		//!< Bufferの設定
		desc.SetHandle("gInput",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::SMAA_EdgeDetection, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}

	{ //!< Blend Weight Calculation

		//!< Process Bufferの準備
		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::SMAA_BlendWeight, context);

		//!< Bufferの設定
		desc.SetHandle("gEdge",   process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::SMAA_BlendWeight, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}

	{ //!< Neighborhood Blending
		//!< Process Bufferの準備
		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		core->SetPipeline(FRenderCoreProcess::CompositeProcess::SMAA_NeighborhoodBlending, context);
		//!< Bufferの設定
		desc.SetHandle("gScene",  process->GetPreviousTexture(2).GetGPUHandleSRV());
		desc.SetHandle("gBlend",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::SMAA_NeighborhoodBlending, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}

}
