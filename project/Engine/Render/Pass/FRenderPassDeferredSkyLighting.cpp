#include "FRenderPassDeferredSkyLighting.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"
#include "../Buffer/FDepthStencilBuffer.h"
#include "../Buffer/FSkyReservoirBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreDirectLight.h"
#include "../Core/FRenderCoreSkyLight.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Components/Component/Light/Environment/SkyLightComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredSkyLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredSkyLighting::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->HasBuffer<FGBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Deferred Sky Lighting]", "FGBuffer is required.");
		return; //!< Bufferが不適格
	}

	if (!config.buffer->HasBuffer<FLightAccumulationBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Deferred Sky Lighting]", "FLightAccumulationBuffer is required.");
		return; //!< Bufferが不適格
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>(); //!< RenderCoreの確保
	// todo: RenderCoreをCoreSkyLight専用にするか検討

	FBaseRenderPass::BeginRenderPass(context, "Deferred Sky Lighting", config);
	
	{ //!< Environment Pass

		FBaseRenderPass::BeginEvent(context, "Environment Pass");

		BeginEnvironmentPass(context, config.buffer);

		RenderEnvironmentSkyLight(context, config);

		RenderEnvironmentSkyAtmosphere(context, config);

		EndEnvironmentPass(context, config.buffer);

		FBaseRenderPass::EndEvent(context);
	}

	if (config.skyVisibility != FRenderConfig::SkyVisibility::None) { //!< Sky Visibility Pass

		FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreSkyLight>(); //!< RenderCoreの確保
		config.buffer->EnsureBuffer<FSkyReservoirBuffer>();                  //!< Bufferの確保

		FBaseRenderPass::BeginEvent(context, "Sky Visibility Pass");

		BeginSkyVisibilityPass(context, config.buffer);

		InitalReservoirSkyVisibility(context, config);

		if (config.skyVisibility == FRenderConfig::SkyVisibility::ReSITR_Preview1) {
			SpatialReservoirSkyVisibility(context, config);
		}

		HistorySkyVisibility(context, config);

		SolveSkyVisibility(context, config);

		EndSkyVisibilityPass(context, config.buffer);

		FBaseRenderPass::EndEvent(context);
	}

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassDeferredSkyLighting::BeginEnvironmentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FLightAccumulationBuffer* lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>();
	FDepthStencilBuffer* depthStencil           = buffer->GetBuffer<FDepthStencilBuffer>();

	static const size_t kBufferCount = 1;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Direct)
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< Light Accumulation Bufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionRenderTarget(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).SetTransitionDepthRead(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

	{ //!< Render Targetの設定

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> handles = {};
		for (size_t i = 0; i < kBufferCount; ++i) {
			handles[i] = buffers[i]->GetCPUHandleRTV();
		}

		commandList->OMSetRenderTargets(
			static_cast<UINT>(handles.size()), handles.data(), false,
			&depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetCPUHandleDSV()
		);
	}

}

void FRenderPassDeferredSkyLighting::EndEnvironmentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FLightAccumulationBuffer* lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>();
	FDepthStencilBuffer* depthStencil           = buffer->GetBuffer<FDepthStencilBuffer>();

	static const size_t kBufferCount = 1;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Direct)
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< GBufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionDefaultState(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).SetTransitionDefaultState(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

}

void FRenderPassDeferredSkyLighting::RenderEnvironmentSkyLight(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::SkyLightEnvironment, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {

		if (!component->IsEnableEnvironment()) {
			return; //!< Environmentが設定されていない場合はスキップ
		}

		//!< componentのparameterの設定
		desc.SetAddress("gParameter", component->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::SkyLightEnvironment, context, desc);
		core->DrawCall(context, 1);
	});

}

void FRenderPassDeferredSkyLighting::RenderEnvironmentSkyAtmosphere(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::SkyAtmosphereEnvironment, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkyAtmosphereComponent>([&](SkyAtmosphereComponent* component) {

		//!< componentのparameterの設定
		desc.SetAddress("gParameter", component->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::SkyAtmosphereEnvironment, context, desc);
		core->DrawCall(context, 1);

	});

}

void FRenderPassDeferredSkyLighting::BeginSkyVisibilityPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FLightAccumulationBuffer* lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>();

	lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).TransitionUnorderedAccess(context);
	//!< Buffer内の確認のため仮でIndirectを使用する.
}

void FRenderPassDeferredSkyLighting::EndSkyVisibilityPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FLightAccumulationBuffer* lightAccumulation = buffer->GetBuffer<FLightAccumulationBuffer>();

	lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).TransitionDefaultState(context);

}

void FRenderPassDeferredSkyLighting::InitalReservoirSkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config) {

	FSkyReservoirBuffer* reservoir       = config.buffer->GetBuffer<FSkyReservoirBuffer>(); //!< SkyReservoirBufferの取得
	FGBuffer* gbuffer                    = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil    = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Initial).TransitionUnordered(context->GetDxCommand()); //!< Reservoirのbarrier設定

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreSkyLight>();
	core->SetPipeline(FRenderCoreSkyLight::Pipeline::InitialReservoir, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene", config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	FRenderCoreSkyLight::Seed<3> seed = {};
	desc.Set32bitConstants("Seed", 3, seed.Data());

	//!< Reservoirの設定
	desc.SetAddress("gInitialReservoir", reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Initial).GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {

		//!< componentのparameterの設定
		desc.SetAddress("gParameter", component->GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreSkyLight::Pipeline::InitialReservoir, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

	});

	reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Initial).TransitionDefault(context->GetDxCommand());

}

void FRenderPassDeferredSkyLighting::SpatialReservoirSkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config) {

	FSkyReservoirBuffer* reservoir       = config.buffer->GetBuffer<FSkyReservoirBuffer>(); //!< SkyReservoirBufferの取得
	FGBuffer* gbuffer                    = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil    = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreSkyLight>();
	core->SetPipeline(FRenderCoreSkyLight::Pipeline::SpatialReservoir, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene", config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	{ //!< 1回目のSpatial

		reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial0).TransitionUnordered(context->GetDxCommand()); //!< Reservoirのbarrier設定

		//!< 回数の設定
		static uint32_t recursion = 1;
		desc.Set32bitConstants("ATrous", 1, &recursion);

		//!< Seedの設定
		FRenderCoreSkyLight::Seed<3> seed = {};
		desc.Set32bitConstants("Seed", 3, seed.Data());

		//!< Reservoirの設定
		desc.SetAddress("gSourceReservoir",      reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Initial).GetGPUVirtualAddress());
		desc.SetAddress("gDestinationReservoir", reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial0).GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreSkyLight::Pipeline::SpatialReservoir, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial0).TransitionDefault(context->GetDxCommand()); //!< Reservoirのbarrier設定
	}

	{ //!< 2回目のSpatial

		reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial1).TransitionUnordered(context->GetDxCommand()); //!< Reservoirのbarrier設定

		//!< 回数の設定
		static uint32_t recursion = 2;
		desc.Set32bitConstants("ATrous", 1, &recursion);

		//!< Seedの設定
		FRenderCoreSkyLight::Seed<3> seed = {};
		desc.Set32bitConstants("Seed", 3, seed.Data());

		//!< Reservoirの設定
		desc.SetAddress("gSourceReservoir",      reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial0).GetGPUVirtualAddress());
		desc.SetAddress("gDestinationReservoir", reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial1).GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreSkyLight::Pipeline::SpatialReservoir, context, desc);
		core->Dispatch(context, config.buffer->GetResolution());

		reservoir->GetReservoir(FSkyReservoirBuffer::LayoutReservoir::Spatial1).TransitionDefault(context->GetDxCommand()); //!< Reservoirのbarrier設定
	}
}

void FRenderPassDeferredSkyLighting::HistorySkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config) {

	FSkyReservoirBuffer::LayoutReservoir type = (config.skyVisibility == FRenderConfig::SkyVisibility::ReSITR_Preview) ? FSkyReservoirBuffer::LayoutReservoir::Initial : FSkyReservoirBuffer::LayoutReservoir::Spatial1; //!< Reservoirの種類の決定

	FSkyReservoirBuffer* reservoir = config.buffer->GetBuffer<FSkyReservoirBuffer>(); //!< SkyReservoirBufferの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreSkyLight>();
	core->SetPipeline(FRenderCoreSkyLight::Pipeline::History, context);

	reservoir->GetBuffer(FSkyReservoirBuffer::LayoutTexture::History).TransitionUnorderedAccess(context); //!< Historyのbarrier設定

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	//!< Reservoirの設定
	desc.SetAddress("gReservoir", reservoir->GetReservoir(type).GetGPUVirtualAddress());

	//!< Historyの設定
	desc.SetHandle("gHistory", reservoir->GetBuffer(FSkyReservoirBuffer::LayoutTexture::History).GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreSkyLight::Pipeline::History, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

	reservoir->GetBuffer(FSkyReservoirBuffer::LayoutTexture::History).TransitionDefaultState(context);

}

void FRenderPassDeferredSkyLighting::SolveSkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config) {

	FSkyReservoirBuffer* reservoir              = config.buffer->GetBuffer<FSkyReservoirBuffer>(); //!< SkyReservoirBufferの取得
	FLightAccumulationBuffer* lightAccumulation = config.buffer->GetBuffer<FLightAccumulationBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreSkyLight>();
	core->SetPipeline(FRenderCoreSkyLight::Pipeline::Solve, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());

	//!< Historyの設定
	desc.SetHandle("gHistory", reservoir->GetBuffer(FSkyReservoirBuffer::LayoutTexture::History).GetGPUHandleSRV());

	//!< Direct Light Accumulationの設定
	desc.SetHandle("gDirect", lightAccumulation->GetBuffer(FLightAccumulationBuffer::Layout::Indirect).GetGPUHandleUAV());
	//!< TestのためIndirectを使用する.

	core->BindComputeBuffer(FRenderCoreSkyLight::Pipeline::Solve, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());
}
