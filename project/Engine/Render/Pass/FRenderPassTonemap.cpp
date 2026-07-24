#include "FRenderPassTonemap.h"
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
// FRenderPassTonemap class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassTonemap::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (!config.option.Test(FRenderConfig::OptionFlag::Tonemap)) {
		return; //!< tonemapオプションが有効でない場合は処理しない
	}

	if (!config.buffer->HasBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Tonemap]", "FMainBuffer not found in RenderTargetBuffer.");
		return; //!< bufferにFMainBufferがない場合は処理しない
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保

	FBaseRenderPass::BeginRenderPass(context, "Tonemap", config);

	{ //!< Tonemap Pass

		BeginTonemapPass(context, config.buffer);

		PassTonemap(context, config);

		EndTonemapPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassTonemap::BeginTonemapPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main = buffer->GetBuffer<FMainBuffer>();

	//!< Scene Bufferのbarrier設定
	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionUnorderedAccess(context);

}

void FRenderPassTonemap::EndTonemapPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main = buffer->GetBuffer<FMainBuffer>();

	//!< Scene Bufferのbarrier設定
	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionDefaultState(context);

}

void FRenderPassTonemap::PassTonemap(const DirectXQueueContext* context, const FRenderConfig& config) {

	FMainBuffer* main = config.buffer->GetBuffer<FMainBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>();
	core->SetPipeline(FRenderCoreProcess::CompositeProcess::Tonemap, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.Set32bitConstants("ColorSpaceBuffer", 1, &config.colorSpace);
	desc.SetHandle("gTexture", main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreProcess::CompositeProcess::Tonemap, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

	// TODO: slope, toe, shoulder... をparameterで調整可能に
}
