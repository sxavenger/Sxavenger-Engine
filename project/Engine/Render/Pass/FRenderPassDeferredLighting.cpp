#include "FRenderPassDeferredLighting.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Components/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/PointLightComponent.h>
#include <Engine/Components/Component/Light/Punctual/SpotLightComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyLightComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredLighting::Render(const DirectXQueueContext* context, const Config& config) {

	if (config.CheckStatus(Config::Status::Geometry_Warning)) {
		return;
	}

	context->BeginEvent(L"RenderPass - DeferredLighting");

	{ //* Direct Lighting
		BeginPassDirectLighting(context, config.buffer);

		context->BeginEvent(L"Direct Lighting Passes");

		PassEmpty(context, config);

		//!< Punctual light
		PassDirectionalLight(context, config);
		PassPointLight(context, config);
		PassSpotLight(context, config);

		//!< Sky light
		PassSkyLight(context, config);
		PassSkyAtmosphere(context, config);

		context->EndEvent();

		EndPassDirectLighting(context, config.buffer);
	}


	if (config.option.Test(FBaseRenderPass::Config::Option::IndirectLighting)) { //* Indirect Lighting

		context->BeginEvent(L"Indirect Lighting Passes");

		{
			BeginPassIndirectLighting(context, config.buffer);

			PassIndirectReservoirReset(context, config);
			PassIndirectReservoirInitialize(context, config);
			PassIndirectReservoirTemporal(context, config);
			PassIndirectReservoirSpatial(context, config);
			//!< FIXME: Spatial Resamplingの修正
			PassIndirectReservoirTexture(context, config);

			EndPassIndirectLighting(context, config.buffer);
		}

		context->EndEvent();


	} else {

		ClearPassIndirect(context, config.buffer);
	}

	if (config.option.Test(FBaseRenderPass::Config::Option::Preview)) {

		context->BeginEvent(L"Probe Lighting Passes");

		PassProbeReservoir(context, config);
		PassProbeUpdate(context, config);
		PassProbeEvaluation(context, config);

		context->EndEvent();
	}

	TransitionLightingPass(context, config);

	context->EndEvent();

}

void FRenderPassDeferredLighting::BeginPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FDepthTexture* depth = buffer->GetDepth();
	depth->TransitionBeginRasterizer(context);

	FBaseTexture* direct = buffer->GetGBuffer(FLightingGBuffer::Layout::Direct);
	direct->TransitionBeginRenderTarget(context);

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

void FRenderPassDeferredLighting::BeginPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::vector<D3D12_RESOURCE_BARRIER> barriers = {
		buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Initialize)->TransitionBeginUnordered(),
		buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->TransitionBeginUnordered(),
		buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Spatial)->TransitionBeginUnordered(),
		buffer->GetLightingGBuffer().GetMoment()->TransitionBeginUnordered(),
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassDeferredLighting::EndPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	static const size_t kBufferCount = 5;

	std::vector<D3D12_RESOURCE_BARRIER> barriers = {
		buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Initialize)->TransitionEndUnordered(),
		buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->TransitionEndUnordered(),
		buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Spatial)->TransitionEndUnordered(),
		buffer->GetLightingGBuffer().GetMoment()->TransitionEndUnordered(),
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassDeferredLighting::ClearPassDirect(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FBaseTexture* direct = buffer->GetGBuffer(FLightingGBuffer::Layout::Direct);
	direct->TransitionBeginRenderTarget(context);
	direct->ClearRenderTarget(context);
	direct->TransitionEndRenderTarget(context);

}

void FRenderPassDeferredLighting::ClearPassIndirect(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::vector<FBaseTexture*> buffers = {
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)
	};

	std::vector<D3D12_RESOURCE_BARRIER> barriers(buffers.size());
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	for (size_t i = 0; i < buffers.size(); ++i) {
		buffers[i]->ClearRenderTarget(context);
	}

	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionEndRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

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

	// deferred parameter
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

	// deferred parameter
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

	// deferred parameter
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

void FRenderPassDeferredLighting::PassSpotLight(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Spot, context, config.buffer->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// deferred parameter
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	sComponentStorage->ForEachActive<SpotLightComponent>([&](SpotLightComponent* component) {

		parameter.SetAddress("gTransforms", component->RequireTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameters", component->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::Spot, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context, 1);

	});

}

void FRenderPassDeferredLighting::PassSkyLight(const DirectXQueueContext* context, const Config& config) {
	//!< TODO: DirectSkyLightingの実装

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// deferred parameter
	parameter.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());

	// BRDF LUT
	parameter.SetHandle("gBRDFLut", FRenderCore::GetInstance()->GetBRDFLut());

	if (!config.option.Test(FBaseRenderPass::Config::Option::IndirectLighting)) {
		//!< IndirectLightingが有効な場合はIndirectLightPassで処理する
		// TODO: DirectSkyLightVisibilityの実装

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
	}

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

void FRenderPassDeferredLighting::PassSkyAtmosphere(const DirectXQueueContext* context, const Config& config) {

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	//* Environment
	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyAtmosphereEnvironment, context, config.buffer->GetSize()
	);

	sComponentStorage->ForEachActive<SkyAtmosphereComponent>([&](SkyAtmosphereComponent* component) {

		// sky light parameter
		parameter.SetAddress("gParameter", component->GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::SkyAtmosphereEnvironment, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);

	});

}

void FRenderPassDeferredLighting::PassIndirectReservoirReset(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetRestir();
	core->SetPipeline(FRenderCoreRestir::Process::Reset, context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	desc.SetAddress("gTemporalReservoir", config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->GetGPUVirtualAddress());
	desc.SetAddress("gSpatialReservoir",  config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Spatial)->GetGPUVirtualAddress());
	desc.SetAddress("gMoment",            config.buffer->GetLightingGBuffer().GetMoment()->GetGPUVirtualAddress());
	desc.SetHandle("gVelocity", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Velocity)->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreRestir::Process::Reset, context, desc);
	core->Dispatch(context, config.buffer->GetSize());

}

void FRenderPassDeferredLighting::PassIndirectReservoirInitialize(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetRestir();
	core->GetContext()->SetStateObject(context->GetDxCommand());

	auto commandList = context->GetCommandList();

	commandList->SetComputeRootUnorderedAccessView(0, config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Initialize)->GetGPUVirtualAddress()); //!< gInitializeReservoir
	commandList->SetComputeRootUnorderedAccessView(1, config.buffer->GetLightingGBuffer().GetMoment()->GetGPUVirtualAddress());                                           //!< gMoment

	//* scene
	commandList->SetComputeRootShaderResourceView(2, config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//* deferred textures
	commandList->SetComputeRootConstantBufferView(3, config.buffer->GetIndexBufferAddress());

	//* camera
	commandList->SetComputeRootConstantBufferView(4, config.camera->GetGPUVirtualAddress());

	//* config
	commandList->SetComputeRoot32BitConstants(5, 2, &config.buffer->GetLightingGBuffer().GetConfig(), 0);
	Seed<uint32_t, 3> seed = {};
	commandList->SetComputeRoot32BitConstants(6, 3, &seed, 0);

	//* light
	// Directional Light
	commandList->SetComputeRootConstantBufferView(7, config.scene->directionalLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(8, config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(9, config.scene->directionalLightParams_->GetGPUVirtualAddress());

	// Point Light
	commandList->SetComputeRootConstantBufferView(10, config.scene->pointLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(11, config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(12, config.scene->pointLightParams_->GetGPUVirtualAddress());

	// Spot Light
	commandList->SetComputeRootConstantBufferView(13, config.scene->spotLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(14, config.scene->spotLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(15, config.scene->spotLightParams_->GetGPUVirtualAddress());

	// Sky Light
	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		commandList->SetComputeRootConstantBufferView(16, component->GetGPUVirtualAddress());
	});
	

	core->GetContext()->DispatchRays(context->GetDxCommand(), config.buffer->GetSize());

}

void FRenderPassDeferredLighting::PassIndirectReservoirTemporal(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetRestir();
	core->SetPipeline(FRenderCoreRestir::Process::Temporal, context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	Seed<uint32_t, 3> seed = {};
	desc.Set32bitConstants("Seed", 3, &seed);
	desc.SetAddress("gInitalizeReservoir", config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Initialize)->GetGPUVirtualAddress());
	desc.SetAddress("gTemporalReservoir",  config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreRestir::Process::Temporal, context, desc);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->Barrier(context->GetDxCommand());

}

void FRenderPassDeferredLighting::PassIndirectReservoirSpatial(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetRestir();
	core->SetPipeline(FRenderCoreRestir::Process::Spatial, context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	Seed<uint32_t, 3> seed = {};
	desc.Set32bitConstants("Seed", 3, &seed);
	desc.SetAddress("gTemporalReservoir",   config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->GetGPUVirtualAddress());
	desc.SetAddress("gSpatialReservoir",    config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Spatial)->GetGPUVirtualAddress());
	desc.SetAddress("gDeferredBufferIndex", config.buffer->GetIndexBufferAddress());
	desc.SetAddress("gScene",               config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreRestir::Process::Spatial, context, desc);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Spatial)->Barrier(context->GetDxCommand());

}

void FRenderPassDeferredLighting::PassIndirectReservoirTexture(const DirectXQueueContext* context, const Config& config) {

	config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetRestir();
	core->SetPipeline(FRenderCoreRestir::Process::Texture, context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	desc.SetAddress("gReservoir",           config.buffer->GetLightingGBuffer().GetReservoir(FLightingGBuffer::Reservoir::Temporal)->GetGPUVirtualAddress());
	desc.SetHandle("gIndirect",             config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleUAV());
	desc.SetAddress("gDeferredBufferIndex", config.buffer->GetIndexBufferAddress());
	desc.SetAddress("gCamera",              config.camera->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreRestir::Process::Texture, context, desc);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->TransitionEndUnordered(context);

}

void FRenderPassDeferredLighting::PassProbeReservoir(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetProbe();
	core->GetContext()->SetStateObject(context->GetDxCommand());

	config.buffer->GetProbeGBuffer().TransitionBeginUnordered(context);

	auto commandList = context->GetCommandList();

	//* GBuffer
	commandList->SetComputeRootDescriptorTable(0, config.buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Sample)->GetGPUHandleUAV()); //!< gProbeSample
	commandList->SetComputeRootDescriptorTable(1, config.buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Reservoir)->GetGPUHandleUAV()); //!< gProbeReservoir
	commandList->SetComputeRootDescriptorTable(2, config.buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Moment)->GetGPUHandleUAV()); //!< gProbeMoment

	//* scene
	commandList->SetComputeRootShaderResourceView(3, config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	FRenderCoreProbe::Config conf = {};
	commandList->SetComputeRoot32BitConstants(4, 8, &conf, 0);

	Seed<uint32_t, 3> seed = {};
	commandList->SetComputeRoot32BitConstants(5, 3, &seed, 0);

	commandList->SetComputeRootConstantBufferView(6, config.camera->GetGPUVirtualAddress());

	//* light
	// Directional Light
	commandList->SetComputeRootConstantBufferView(7, config.scene->directionalLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(8, config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(9, config.scene->directionalLightParams_->GetGPUVirtualAddress());

	// Point Light
	commandList->SetComputeRootConstantBufferView(10, config.scene->pointLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(11, config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(12, config.scene->pointLightParams_->GetGPUVirtualAddress());

	// Spot Light
	commandList->SetComputeRootConstantBufferView(13, config.scene->spotLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(14, config.scene->spotLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(15, config.scene->spotLightParams_->GetGPUVirtualAddress());

	// Sky Light
	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		commandList->SetComputeRootConstantBufferView(16, component->GetGPUVirtualAddress());
	});

	core->GetContext()->DispatchRays(context->GetDxCommand(), config.buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Sample)->GetSize());

	config.buffer->GetProbeGBuffer().TransitionEndUnordered(context);

}

void FRenderPassDeferredLighting::PassProbeUpdate(const DirectXQueueContext* context, const Config& config) {

	auto buffer = config.buffer;

	auto core = FRenderCore::GetInstance()->GetProbe();
	core->SetPipeline(FRenderCoreProbe::Process::Update, context);

	buffer->GetProbeGBuffer().TransitionBeginUnordered(context);

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gProbeSample", buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Sample)->GetGPUHandleUAV());
	desc.SetHandle("gProbeReservoir", buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Reservoir)->GetGPUHandleUAV());
	desc.SetHandle("gProbeMoment", buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Moment)->GetGPUHandleUAV());
	desc.SetHandle("gProbeIrradiance", buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Irradiance)->GetGPUHandleUAV());

	FRenderCoreProbe::Config conf = {};
	desc.Set32bitConstants("Config", 8, &conf);

	core->BindComputeBuffer(FRenderCoreProbe::Process::Update, context, desc);
	core->Dispatch(context, buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Sample)->GetSize());

	buffer->GetProbeGBuffer().TransitionEndUnordered(context);
}

void FRenderPassDeferredLighting::PassProbeEvaluation(const DirectXQueueContext* context, const Config& config) {

	auto buffer = config.buffer;

	auto core = FRenderCore::GetInstance()->GetProbe();
	core->SetPipeline(FRenderCoreProbe::Process::Evaluate, context);

	buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->TransitionBeginUnordered(context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());

	desc.SetHandle("gIndirect",buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleUAV());

	desc.SetAddress("gDeferredBufferIndex", buffer->GetIndexBufferAddress());

	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	FRenderCoreProbe::Config conf = {};
	desc.Set32bitConstants("Config", 8, &conf);
	desc.SetHandle("gProbeSample",    buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Sample)->GetGPUHandleSRV());
	desc.SetHandle("gProbeReservoir", buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Reservoir)->GetGPUHandleSRV());
	desc.SetHandle("gProbeIrradiance", buffer->GetProbeGBuffer().GetGBuffer(FProbeGBuffer::Probe::Irradiance)->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreProbe::Process::Evaluate, context, desc);
	core->Dispatch(context, buffer->GetSize());

	buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->TransitionEndUnordered(context);
}

void FRenderPassDeferredLighting::TransitionLightingPass(const DirectXQueueContext* context, const Config& config) {

	config.buffer->TransitionBeginUnorderedMainScene(context);

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

	config.buffer->TransitionEndUnorderedMainScene(context);

}
