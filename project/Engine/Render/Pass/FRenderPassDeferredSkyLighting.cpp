#include "FRenderPassDeferredSkyLighting.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"
#include "../Buffer/FDepthStencilBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreDirectLight.h"

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
	FDepthStencilBuffer* depthStencil          = buffer->GetBuffer<FDepthStencilBuffer>();

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
		desc.SetAddress("gParameters", component->GetGPUVirtualAddress());

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
