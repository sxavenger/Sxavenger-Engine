#include "FRenderPassDeferredGlobalIllumination.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"
#include "../Buffer/FReservoirBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreReSTIR.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredGlobalIllumination class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredGlobalIllumination::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	FLightAccumulationBuffer* lightAccumulation = config.buffer->GetBuffer<FLightAccumulationBuffer>(); //!< Bufferの確保
	lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).ClearRenderTarget(context); //!< 間接光のバッファをクリア

	if (config.globalIllumination == FRenderConfig::GlobalIllumination::None) {
		return; //!< 間接光が無効
	}

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->HasBuffer<FGBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Deferred Global Illumination]", "FGBuffer is required.");
		return; //!< Bufferが不適格
	}

	config.buffer->EnsureBuffer<FLightAccumulationBuffer>(); //!< Bufferの確保
	//!< Buffer, RenderCoreは各Passで必要に応じて確保する.

	FBaseRenderPass::BeginRenderPass(context, "Deferred Global Illumination", config);

	{ //!< Global Illumination Pass

		BeginGlobalIlluminationPass(context, config.buffer);

		switch (config.globalIllumination) {
			case FRenderConfig::GlobalIllumination::ReSTIR_GI:
				BeginPassReSTIR(context, config.buffer);
				PassReSTIRResetReservoir(context, config);
				PassReSTIRInitialReservoir(context, config);
				PassReSTIRTemporalReuse(context, config);
				PassReSTIRSolve(context, config);
				EndPassReSTIR(context, config.buffer);
				break;
		}

		EndGlobalIlluminationPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassDeferredGlobalIllumination::BeginGlobalIlluminationPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	auto lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>(); //!< LightAccumulationBufferの取得
	lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).TransitionUnorderedAccess(context);
}

void FRenderPassDeferredGlobalIllumination::EndGlobalIlluminationPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	auto lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>(); //!< LightAccumulationBufferの取得
	lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).TransitionDefaultState(context);
}

void FRenderPassDeferredGlobalIllumination::BeginPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	buffer->EnsureBuffer<FReservoirBuffer>(); //!< Bufferの確保

	// HACK: Scene概念が変更されない前提.
	// ShaderTableの保持がRenderCore側のため, Sceneが変更できない.
	StreamLogger::AssertA(FRenderCore::GetInstance()->HasRenderCore<FRenderCoreReSTIR>(), "[FRenderPass - Deferred Indirect Lighting]", "FRenderCoreReSTIR is required. (hack methods.)"); //!< RenderCoreが存在しない.
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得. (TLASの更新完了済み)

	FBaseRenderPass::BeginEvent(context, "ReSTIR Global Illumination Pass");
}

void FRenderPassDeferredGlobalIllumination::EndPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	FBaseRenderPass::EndEvent(context);
	buffer;
}

void FRenderPassDeferredGlobalIllumination::PassReSTIRResetReservoir(const DirectXQueueContext* context, const FRenderConfig& config) {

	FReservoirBuffer* reservoir = config.buffer->GetBuffer<FReservoirBuffer>(); //!< Reservoirの取得
	FGBuffer* gbuffer = config.buffer->GetBuffer<FGBuffer>();                   //!< GBufferの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得.
	core->SetPipeline(FRenderCoreReSTIR::Process::Reset, context);

	reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).TransitionUnordered(context->GetDxCommand());
	reservoir->GetReservoir(FReservoirBuffer::Layout::Spatial).TransitionUnordered(context->GetDxCommand());
	reservoir->GetMoment().TransitionUnordered(context->GetDxCommand());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	//!< reservoirの設定
	desc.SetAddress("gTemporalReservoir", reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).GetGPUVirtualAddress());
	desc.SetAddress("gSpatialReservoir",  reservoir->GetReservoir(FReservoirBuffer::Layout::Spatial).GetGPUVirtualAddress());
	desc.SetAddress("gMoment",            reservoir->GetMoment().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gMotionVector", gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreReSTIR::Process::Reset, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

	reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).TransitionDefault(context->GetDxCommand());
	reservoir->GetReservoir(FReservoirBuffer::Layout::Spatial).TransitionDefault(context->GetDxCommand());
	reservoir->GetMoment().TransitionDefault(context->GetDxCommand());

}

void FRenderPassDeferredGlobalIllumination::PassReSTIRInitialReservoir(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto commandList = context->GetCommandList(); //!< CommandListの取得

	FReservoirBuffer* reservoir        = config.buffer->GetBuffer<FReservoirBuffer>(); //!< Reservoirの取得
	FGBuffer* gbuffer                  = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilTexture* depthStencil = config.buffer->GetDepthStencil(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得.
	core->GetContext()->SetStateObject(context->GetDxCommand());

	reservoir->GetReservoir(FReservoirBuffer::Layout::Initial).TransitionUnordered(context->GetDxCommand());
	reservoir->GetMoment().TransitionUnordered(context->GetDxCommand());

	//* output buffer
	commandList->SetComputeRootUnorderedAccessView(0, reservoir->GetReservoir(FReservoirBuffer::Layout::Initial).GetGPUVirtualAddress()); //!< gInitialReservoir
	commandList->SetComputeRootUnorderedAccessView(1, reservoir->GetMoment().GetGPUVirtualAddress());                                     //!< gMoment
	
	//* scene
	commandList->SetComputeRootShaderResourceView(2, config.scene->GetTopLevelAS().GetGPUVirtualAddress()); //!< gScene

	//* camera
	commandList->SetComputeRootConstantBufferView(3, config.camera->GetGPUVirtualAddress());

	//* setting
	FRenderCoreReSTIR::Setting setting = {}; //!< Default設定として使用.
	commandList->SetComputeRoot32BitConstants(4, 2, &setting, 0); //!< gSetting

	//* seed
	Seed<uint32_t, 3> seed = {};
	commandList->SetComputeRoot32BitConstants(5, 3, &seed, 0); //!< gSeed

	//* GBuffer
	commandList->SetComputeRootDescriptorTable(6, depthStencil->GetGPUHandleSRV());                                     //!< gDepth
	commandList->SetComputeRootDescriptorTable(7, gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());      //!< gAlbedo
	commandList->SetComputeRootDescriptorTable(8, gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());      //!< gNormal
	commandList->SetComputeRootDescriptorTable(9, gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV()); //!< gMaterialARM

	//* light
	// Directional Light
	FScene::LightAddress directionalLightAddress = config.scene->GetDirectionalLightAddress();
	commandList->SetComputeRoot32BitConstants(10, 1, &directionalLightAddress.count, 0);
	commandList->SetComputeRootShaderResourceView(11, directionalLightAddress.transforms);
	commandList->SetComputeRootShaderResourceView(12, directionalLightAddress.parameters);

	// Point Light
	FScene::LightAddress pointLightAddress = config.scene->GetPointLightAddress();
	commandList->SetComputeRoot32BitConstants(13, 1, &pointLightAddress.count, 0);
	commandList->SetComputeRootShaderResourceView(14, pointLightAddress.transforms);
	commandList->SetComputeRootShaderResourceView(15, pointLightAddress.parameters);

	// Spot Light
	FScene::LightAddress spotLightAddress = config.scene->GetSpotLightAddress();
	commandList->SetComputeRoot32BitConstants(16, 1, &spotLightAddress.count, 0);
	commandList->SetComputeRootShaderResourceView(17, spotLightAddress.transforms);
	commandList->SetComputeRootShaderResourceView(18, spotLightAddress.parameters);

	core->GetContext()->DispatchRays(context->GetDxCommand(), config.buffer->GetResolution());

	reservoir->GetReservoir(FReservoirBuffer::Layout::Initial).TransitionDefault(context->GetDxCommand());
	reservoir->GetMoment().TransitionDefault(context->GetDxCommand());
}

void FRenderPassDeferredGlobalIllumination::PassReSTIRTemporalReuse(const DirectXQueueContext* context, const FRenderConfig& config) {

	FReservoirBuffer* reservoir = config.buffer->GetBuffer<FReservoirBuffer>(); //!< Reservoirの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得.
	core->SetPipeline(FRenderCoreReSTIR::Process::Temporal, context);

	reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).TransitionUnordered(context->GetDxCommand());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	Seed<uint32_t, 3> seed = {};
	desc.Set32bitConstants("Seed", 3, &seed);

	//!< reservoirの設定
	desc.SetAddress("gInitialReservoir", reservoir->GetReservoir(FReservoirBuffer::Layout::Initial).GetGPUVirtualAddress());
	desc.SetAddress("gTemporalReservoir", reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreReSTIR::Process::Temporal, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

	reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).TransitionDefault(context->GetDxCommand());
}

void FRenderPassDeferredGlobalIllumination::PassReSTIRSolve(const DirectXQueueContext* context, const FRenderConfig& config) {

	FReservoirBuffer* reservoir                 = config.buffer->GetBuffer<FReservoirBuffer>();         //!< Reservoirの取得
	FDepthStencilTexture* depthStencil          = config.buffer->GetDepthStencil();                     //!< DepthStencilの取得
	FLightAccumulationBuffer* lightAccumulation = config.buffer->GetBuffer<FLightAccumulationBuffer>(); //!< LightAccumulationBufferの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得.
	core->SetPipeline(FRenderCoreReSTIR::Process::Solve, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gReservoir", reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).GetGPUVirtualAddress());
	desc.SetAddress("gCamera",    config.camera->GetGPUVirtualAddress());
	desc.SetHandle("gDepth",      depthStencil->GetGPUHandleSRV());
	desc.SetHandle("gIndirect",   lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreReSTIR::Process::Solve, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

}
