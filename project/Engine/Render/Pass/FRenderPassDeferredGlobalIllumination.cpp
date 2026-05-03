#include "FRenderPassDeferredGlobalIllumination.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"
#include "../Buffer/FReservoirBuffer.h"
#include "../Buffer/FScreenSpaceProbeBuffer.h"
#include "../Buffer/FDepthStencilBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreReSTIR.h"
#include "../Core/FRenderCoreLuxGlobalIllumination.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredGlobalIllumination class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredGlobalIllumination::Init() {
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>();
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreLuxGlobalIllumination>();
}

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

		FBaseRenderPass::BeginEvent(context, std::format("Global Illumination | {}", magic_enum::enum_name(config.globalIllumination)));

		switch (config.globalIllumination) {
			case FRenderConfig::GlobalIllumination::Lux:
				BeginPassLux(context, config.buffer);
				PassLuxProbeTrace(context, config);
				PassLuxHistory(context, config);
				PassLuxSolve(context, config);
				EndPassLux(context, config.buffer);
				break;

			case FRenderConfig::GlobalIllumination::ReSTIR_GI:
				BeginPassReSTIR(context, config.buffer);
				PassReSTIRResetReservoir(context, config);
				PassReSTIRInitialReservoir(context, config);
				PassReSTIRTemporalReuse(context, config);
				PassReSTIRSolve(context, config);
				EndPassReSTIR(context, config.buffer);
				break;

			default:
				RuntimeLogger::LogWarning(
					"[FRenderPass - Deferred Global Illumination]",
					std::format("unsupported global illumination. name: {}", magic_enum::enum_name(config.globalIllumination))
				);
				break;
		}

		FBaseRenderPass::EndEvent(context);

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

void FRenderPassDeferredGlobalIllumination::BeginPassLux(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	buffer->EnsureBuffer<FScreenSpaceProbeBuffer>(); //!< Bufferの確保

	// HACK: Scene概念が変更されない前提.
	// ShaderTableの保持がRenderCore側のため, Sceneが変更できない.
	StreamLogger::AssertA(FRenderCore::GetInstance()->HasRenderCore<FRenderCoreLuxGlobalIllumination>(), "[FRenderPass - Deferred Global Illumination]", "FRenderCoreLuxGlobalIllumination is required. (hack methods.)"); //!< RenderCoreが存在しない.
	context;
}

void FRenderPassDeferredGlobalIllumination::EndPassLux(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	context;
	buffer;
}

void FRenderPassDeferredGlobalIllumination::PassLuxProbeTrace(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto commandList = context->GetCommandList(); //!< CommandListの取得

	FScreenSpaceProbeBuffer* probe    = config.buffer->GetBuffer<FScreenSpaceProbeBuffer>(); //!< ScreenSpaceProbeの取得
	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreLuxGlobalIllumination>(); //!< RenderCoreの取得.
	core->GetContext()->SetStateObject(context->GetDxCommand());

	probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::BRDFRadianceCache).TransitionUnorderedAccess(context);
	probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::Moment).TransitionUnorderedAccess(context);

	//* cache
	commandList->SetComputeRootDescriptorTable(0, probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::BRDFRadianceCache).GetGPUHandleUAV()); //!< gBRDFRadianceCache
	commandList->SetComputeRootDescriptorTable(1, probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::Moment).GetGPUHandleUAV());            //!< gMoment

	//* scene
	commandList->SetComputeRootShaderResourceView(2, config.scene->GetTopLevelAS().GetGPUVirtualAddress()); //!< gScene

	//* camera
	commandList->SetComputeRootConstantBufferView(3, config.camera->GetGPUVirtualAddress()); //!< gCamera

	//* setting
	static const FRenderCoreLuxGlobalIllumination::Setting setting = {}; //!< Default設定として使用.
	commandList->SetComputeRoot32BitConstants(4, 5, &setting, 0); //!< gSetting

	//* resolution
	commandList->SetComputeRoot32BitConstants(5, 2, &config.buffer->GetResolution(), 0); //!< Resolution

	//* GBuffer
	commandList->SetComputeRootDescriptorTable(6, depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV()); //!< gDepth
	commandList->SetComputeRootDescriptorTable(7, gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());                //!< gAlbedo
	commandList->SetComputeRootDescriptorTable(8, gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());                //!< gNormal
	commandList->SetComputeRootDescriptorTable(9, gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());           //!< gMaterialARM

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

	core->GetContext()->DispatchRays(context->GetDxCommand(), setting.CalculateResolution(config.buffer->GetResolution()));

	probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::BRDFRadianceCache).TransitionDefaultState(context);
	probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::Moment).TransitionDefaultState(context);
}

void FRenderPassDeferredGlobalIllumination::PassLuxHistory(const DirectXQueueContext* context, const FRenderConfig& config) {

	FScreenSpaceProbeBuffer* probe    = config.buffer->GetBuffer<FScreenSpaceProbeBuffer>(); //!< ScreenSpaceProbeの取得
	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreLuxGlobalIllumination>(); //!< RenderCoreの取得.
	core->SetPipeline(FRenderCoreLuxGlobalIllumination::Process::History, context);

	probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::History).TransitionUnorderedAccess(context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	static const FRenderCoreLuxGlobalIllumination::Setting setting = {}; //!< Default設定として使用.
	desc.Set32bitConstants("Setting", 5, &setting); //!< Setting

	//!< cacheの設定
	desc.SetHandle("gBRDFRadianceCache", probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::BRDFRadianceCache).GetGPUHandleSRV());
	desc.SetHandle("gMoment",            probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::Moment).GetGPUHandleSRV());

	//!< historyの設定
	desc.SetHandle("gHistory", probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::History).GetGPUHandleUAV());

	//!< cameraの設定
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreLuxGlobalIllumination::Process::History, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

	probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::History).TransitionDefaultState(context);
}

void FRenderPassDeferredGlobalIllumination::PassLuxSolve(const DirectXQueueContext* context, const FRenderConfig& config) {

	FScreenSpaceProbeBuffer* probe              = config.buffer->GetBuffer<FScreenSpaceProbeBuffer>(); //!< ScreenSpaceProbeの取得
	FLightAccumulationBuffer* lightAccumulation = config.buffer->GetBuffer<FLightAccumulationBuffer>(); //!< LightAccumulationBufferの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreLuxGlobalIllumination>(); //!< RenderCoreの取得.
	core->SetPipeline(FRenderCoreLuxGlobalIllumination::Process::Solve, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	static const FRenderCoreLuxGlobalIllumination::Setting setting = {}; //!< Default設定として使用.
	desc.Set32bitConstants("Setting", 5, &setting); //!< Setting

	//!< historyの設定
	desc.SetHandle("gHistory", probe->GetBuffer(FScreenSpaceProbeBuffer::Layout::History).GetGPUHandleSRV());

	//!< light accumulationの設定
	desc.SetHandle("gIndirect", lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreLuxGlobalIllumination::Process::Solve, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

}

void FRenderPassDeferredGlobalIllumination::BeginPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	buffer->EnsureBuffer<FReservoirBuffer>(); //!< Bufferの確保

	// HACK: Scene概念が変更されない前提.
	// ShaderTableの保持がRenderCore側のため, Sceneが変更できない.
	StreamLogger::AssertA(FRenderCore::GetInstance()->HasRenderCore<FRenderCoreReSTIR>(), "[FRenderPass - Deferred Global Illumination]", "FRenderCoreReSTIR is required. (hack methods.)"); //!< RenderCoreが存在しない.
	context;
}

void FRenderPassDeferredGlobalIllumination::EndPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	context;
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

	FReservoirBuffer* reservoir       = config.buffer->GetBuffer<FReservoirBuffer>(); //!< Reservoirの取得
	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得.
	core->GetContext()->SetStateObject(context->GetDxCommand());

	reservoir->GetReservoir(FReservoirBuffer::Layout::Initial).TransitionUnordered(context->GetDxCommand());
	reservoir->GetMoment().TransitionUnordered(context->GetDxCommand());

	//* output buffers
	commandList->SetComputeRootUnorderedAccessView(0, reservoir->GetReservoir(FReservoirBuffer::Layout::Initial).GetGPUVirtualAddress()); //!< gInitialReservoir
	commandList->SetComputeRootUnorderedAccessView(1, reservoir->GetMoment().GetGPUVirtualAddress());                                     //!< gMoment
	
	//* scene
	commandList->SetComputeRootShaderResourceView(2, config.scene->GetTopLevelAS().GetGPUVirtualAddress()); //!< gScene

	//* camera
	commandList->SetComputeRootConstantBufferView(3, config.camera->GetGPUVirtualAddress());

	//* setting
	static const FRenderCoreReSTIR::Setting setting = {}; //!< Default設定として使用.
	commandList->SetComputeRoot32BitConstants(4, 2, &setting, 0); //!< gSetting

	//* seed
	const Seed<uint32_t, 3> seed = {};
	commandList->SetComputeRoot32BitConstants(5, 3, &seed, 0); //!< gSeed

	//* GBuffer
	commandList->SetComputeRootDescriptorTable(6, depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV()); //!< gDepth
	commandList->SetComputeRootDescriptorTable(7, gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());                //!< gAlbedo
	commandList->SetComputeRootDescriptorTable(8, gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());                //!< gNormal
	commandList->SetComputeRootDescriptorTable(9, gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());           //!< gMaterialARM

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
	FDepthStencilBuffer* depthStencil           = config.buffer->GetBuffer<FDepthStencilBuffer>();                     //!< DepthStencilの取得
	FLightAccumulationBuffer* lightAccumulation = config.buffer->GetBuffer<FLightAccumulationBuffer>(); //!< LightAccumulationBufferの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreReSTIR>(); //!< RenderCoreの取得.
	core->SetPipeline(FRenderCoreReSTIR::Process::Solve, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gReservoir", reservoir->GetReservoir(FReservoirBuffer::Layout::Temporal).GetGPUVirtualAddress());
	desc.SetAddress("gCamera",    config.camera->GetGPUVirtualAddress());
	desc.SetHandle("gDepth",      depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gIndirect",   lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreReSTIR::Process::Solve, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

}
