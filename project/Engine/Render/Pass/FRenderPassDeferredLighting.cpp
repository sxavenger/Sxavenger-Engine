#include "FRenderPassDeferredLighting.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredLighting::Render(const DirectXQueueContext* context, const Config& config) {
	// TODO: ShaderとPipelineの調整
	// DepthをPipelineで参照する形に変更

	{ //* Direct Lighting
		config.buffer->BeginRenderTargetLightingDirect(context);

		PassEmpty(context, config);

		//!< Punctual light
		PassDirectionalLight(context, config);
		PassPointLight(context, config);
		// todo: Spot Light

		//!< Sky light
		PassSkyLight(context, config);

		config.buffer->EndRenderTargetLightingDirect(context);
	}


	//if (config.isEnableIndirectLighting) { //* Indirect Lighting
	//	ProcessLightingPassIndirect(context, config);
	//	ProcessLightingPassIndirectDenoiser(context, config);
	//}

	LightingPassTransition(context, config);

}

void FRenderPassDeferredLighting::PassEmpty(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Empty, context, config.buffer->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth",    config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Empty, context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context);

}

void FRenderPassDeferredLighting::PassDirectionalLight(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Directional, context, config.buffer->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene", config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth", config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {

		parameter.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameters", component->GetParameterBufferAddress());
		parameter.SetAddress("gShadows",    component->GetShadowBufferAddress());

		// TODO: Parameterの中にShadowを含める

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::Directional, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context, 1);

	});

}

void FRenderPassDeferredLighting::PassPointLight(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Point, context, config.buffer->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth",    config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {

		parameter.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameters", component->GetParameterBufferAddress());
		parameter.SetAddress("gShadows",    component->GetShadowBufferAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::Point, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context, 1);

	});

}

void FRenderPassDeferredLighting::PassSkyLight(const DirectXQueueContext* context, const Config& config) {

	if (config.isEnableIndirectLighting) {
		return; //!< Indirect Lightingで処理するため, スキップ
	}

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyLight, context, config.buffer->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth",    config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	// BRDF LUT
	parameter.SetHandle("gBRDFLut", FRenderCore::GetInstance()->GetBRDFLut());

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {

		if (!component->IsEnableIrradiance() || !component->IsEnableRadiance()) {
			return; //!< IrradianceやRadianceが設定されていない場合はスキップ
		}

		// sky light parameter
		parameter.SetAddress("gParameter", component->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::SkyLight, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});

	// TODO: 背景にskyboxを描画する

}

void FRenderPassDeferredLighting::LightingPassTransition(const DirectXQueueContext* context, const Config& config) {

	config.buffer->BeginUnorderedMainScene(context);

	auto core = FRenderCore::GetInstance()->GetTransition();

	core->SetPipeline(FRenderCoreTransition::Transition::LightingTransition, context);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// input
	parameter.SetHandle("gDirect", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Direct)->GetGPUHandleSRV());
	parameter.SetHandle("gIndirect", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleSRV());

	// output
	parameter.SetHandle("gOutput", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::LightingTransition, context, parameter);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->EndUnorderedMainScene(context);

}
