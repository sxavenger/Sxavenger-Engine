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
		BeginPassDirectLighting(context, config.buffer);

		PassEmpty(context, config);

		//!< Punctual light
		PassDirectionalLight(context, config);
		PassPointLight(context, config);
		// todo: Spot Light

		//!< Sky light
		PassSkyLight(context, config);

		EndPassDirectLighting(context, config.buffer);
	}


	//if (config.isEnableIndirectLighting) { //* Indirect Lighting
	//	ProcessLightingPassIndirect(context, config);
	//	ProcessLightingPassIndirectDenoiser(context, config);
	//}

	LightingPassTransition(context, config);

}

void FRenderPassDeferredLighting::BeginPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	FDepthTexture* depth = buffer->GetDepth();
	depth->TransitionBeginRasterizer(context);

	FBaseTexture* direct = buffer->GetGBuffer(FLightingGBuffer::Layout::Direct);
	direct->TransitionBeginRenderTarget(context);

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = direct->TransitionBeginRenderTarget();

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> handles = {};
	handles[0] = direct->GetCPUHandleRTV();

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depth->GetRasterizerCPUHandleDSV()
	);

	direct->ClearRenderTarget(context);
}

void FRenderPassDeferredLighting::EndPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	FDepthTexture* depth = buffer->GetDepth();
	depth->TransitionEndRasterizer(context);

	FBaseTexture* direct = buffer->GetGBuffer(FLightingGBuffer::Layout::Direct);
	direct->TransitionEndRenderTarget(context);
}

void FRenderPassDeferredLighting::PassEmpty(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Empty, context, config.buffer->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// deferred paraemter
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
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// deferred paraemter
	parameter.SetHandle("gAlbedo", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {

		parameter.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameters", component->GetGPUVirtualAddress());

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
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// deferred paraemter
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {

		parameter.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameters", component->GetGPUVirtualAddress());

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

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// deferred paraemter
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	// BRDF LUT
	parameter.SetHandle("gBRDFLut", FRenderCore::GetInstance()->GetBRDFLut());

	//* Irradiance/Radiance
	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyLight, context, config.buffer->GetSize()
	);

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

	//* Environment
	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyLightEnvironment, context, config.buffer->GetSize()
	);

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {

		if (!component->IsEnableEnvironment()) {
			return; //!< Environmentが設定されていない場合はスキップ
		}

		// sky light parameter
		parameter.SetAddress("gParameter", component->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::SkyLightEnvironment, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);

	});

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
