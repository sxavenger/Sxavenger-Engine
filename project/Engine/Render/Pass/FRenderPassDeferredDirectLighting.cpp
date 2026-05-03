#include "FRenderPassDeferredDirectLighting.h"
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
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/PointLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/SpotLightComponent.h>
#include <Engine/Components/Component/Light/Rect/RectLightComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyLightComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredDirectLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredDirectLighting::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->HasBuffer<FGBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Deferred Direct Lighting]", "FGBuffer is required.");
		return; //!< Bufferが不適格
	}

	config.buffer->EnsureBuffer<FLightAccumulationBuffer>();                //!< Bufferの確保
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>(); //!< RenderCoreの確保

	FBaseRenderPass::BeginRenderPass(context, "Deferred Direct Lighting", config);

	{ //!< Direct Lighting Pass

		BeginDirectLightingPass(context, config.buffer);

		//!< Unlit
		PassUnlit(context, config);

		//!< Punctual Light
		PassDirectionalLight(context, config);
		PassPointLight(context, config);
		PassSpotLight(context, config);

		//!< Rect Light
		PassRectLight(context, config);

		EndDirectLightingPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassDeferredDirectLighting::BeginDirectLightingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

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

	for (size_t i = 0; i < kBufferCount; ++i) {
		buffers[i]->ClearRenderTarget(context);
	}
}

void FRenderPassDeferredDirectLighting::EndDirectLightingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

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

void FRenderPassDeferredDirectLighting::PassUnlit(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::Unlit, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::Unlit, context, desc);
	core->DrawCall(context, 1);
}

void FRenderPassDeferredDirectLighting::PassDirectionalLight(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::Directional, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {

		//!< componentのparameterの設定
		desc.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		desc.SetAddress("gParameters", component->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::Directional, context, desc);
		core->DrawCall(context, 1);
	});

}

void FRenderPassDeferredDirectLighting::PassPointLight(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::Point, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {

		//!< componentのparameterの設定
		desc.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		desc.SetAddress("gParameters", component->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::Point, context, desc);
		core->DrawCall(context, 1);
	});

}

void FRenderPassDeferredDirectLighting::PassSpotLight(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::Spot, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	sComponentStorage->ForEachActive<SpotLightComponent>([&](SpotLightComponent* component) {

		//!< componentのparameterの設定
		desc.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		desc.SetAddress("gParameters", component->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::Spot, context, desc);
		core->DrawCall(context, 1);
	});

}

void FRenderPassDeferredDirectLighting::PassRectLight(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();
	core->SetPipeline(FRenderCoreDirectLight::Type::Rect, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	sComponentStorage->ForEachActive<RectLightComponent>([&](RectLightComponent* component) {

		//!< componentのparameterの設定
		desc.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		desc.SetAddress("gParameters", component->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::Rect, context, desc);
		core->DrawCall(context, 1);
	});

}

void FRenderPassDeferredDirectLighting::PassSkyLight(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	//!< BRDFの設定
	desc.SetHandle("gBRDFLUT", FRenderCore::GetInstance()->GetBRDFLut());

	{ //!< Environment描画

		core->SetPipeline(FRenderCoreDirectLight::Type::SkyLightEnvironment, context, config.buffer->GetResolution());

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

	// Indirect有効時, IndirectLightPassで処理する

	{ //!< Lighting

		core->SetPipeline(FRenderCoreDirectLight::Type::SkyLight, context, config.buffer->GetResolution());

		sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {

			if (!component->IsEnableIrradiance() || !component->IsEnableRadiance()) {
				return; //!< IrradianceやRadianceが設定されていない場合はスキップ
			}

			//!< componentのparameterの設定
			desc.SetAddress("gParameters", component->GetGPUVirtualAddress());

			core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::SkyLight, context, desc);
			core->DrawCall(context);
		});

	}
	// todo: SkyLightVisibilityの実装

}

void FRenderPassDeferredDirectLighting::PassSkyAtmosphere(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>(); //!< GBufferの取得
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>(); //!< DepthStencilの取得

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreDirectLight>();

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//!< GBufferの設定
	desc.SetHandle("gDepth",       depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetHandle("gAlbedo",      gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
	desc.SetHandle("gNormal",      gbuffer->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
	desc.SetHandle("gMaterialARM", gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM).GetGPUHandleSRV());

	{ //!< Environment描画

		core->SetPipeline(FRenderCoreDirectLight::Type::SkyAtmosphereEnvironment, context, config.buffer->GetResolution());

		sComponentStorage->ForEachActive<SkyAtmosphereComponent>([&](SkyAtmosphereComponent* component) {

			//!< componentのparameterの設定
			desc.SetAddress("gParameter", component->GetGPUVirtualAddress());

			core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::SkyAtmosphereEnvironment, context, desc);
			core->DrawCall(context, 1);

		});
	}

	{ //!< Aerial

		core->SetPipeline(FRenderCoreDirectLight::Type::SkyAtmosphereAerial, context, config.buffer->GetResolution());

		sComponentStorage->ForEachActive<SkyAtmosphereComponent>([&](SkyAtmosphereComponent* component) {

			//!< componentのparameterの設定
			desc.SetAddress("gParameter", component->GetGPUVirtualAddress());

			core->BindGraphicsBuffer(FRenderCoreDirectLight::Type::SkyAtmosphereAerial, context, desc);
			//core->DrawCall(context, 1);

			//!< FIXME: Sampleと結果が一致しない.
		});

	}
}
