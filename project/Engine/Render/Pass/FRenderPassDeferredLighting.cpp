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
#include <Engine/Component/Components/Light/Punctual/SpotLightComponent.h>
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredLighting::Render(const DirectXQueueContext* context, const Config& config) {
	
	if (config.CheckStatus(Config::Status::Geometry_Warning)) {
		ClearPassDirect(context, config.buffer);
		ClearPassIndirect(context, config.buffer);
		return;
	}

	{ //* Direct Lighting
		BeginPassDirectLighting(context, config.buffer);

		PassEmpty(context, config);

		//!< Punctual light
		PassDirectionalLight(context, config);
		PassPointLight(context, config);
		PassSpotLight(context, config);

		//!< Sky light
		PassSkyLight(context, config);

		EndPassDirectLighting(context, config.buffer);
	}


	if (config.isEnableIndirectLighting) { //* Indirect Lighting

		{
			BeginPassIndirectLighting(context, config.buffer);

			//PassIndirectMomentTranslate(context, config);

			PassIndirectLight(context, config);

			EndPassIndirectLighting(context, config.buffer);
		}
		
		{

			TransitionUpscaleIndirectBuffer(context, config);

			PassIndirectDenoiser(context, config);

		}
		

	} else {

		ClearPassIndirect(context, config.buffer);
	}

	TransitionLightingPass(context, config);

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

	static const size_t kBufferCount = 5;

	std::array<FBaseTexture*, kBufferCount> buffers = {
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Specular),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Diffuse),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Specular),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)
	};

	std::array<D3D12_RESOURCE_BARRIER, kBufferCount> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionBeginUnordered();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassDeferredLighting::EndPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	static const size_t kBufferCount = 5;

	std::array<FBaseTexture*, kBufferCount> buffers = {
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Specular),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Diffuse),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Specular),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)
	};

	std::array<D3D12_RESOURCE_BARRIER, kBufferCount> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionEndUnordered();
	}

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

	static const size_t kBufferCount = 6;

	std::array<FBaseTexture*, kBufferCount> buffers = {
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Specular),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Diffuse),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Specular),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment),
		buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)
	};

	std::array<D3D12_RESOURCE_BARRIER, kBufferCount> barriers = {};
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

void FRenderPassDeferredLighting::PassSpotLight(const DirectXQueueContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Spot, context, config.buffer->GetSize()
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

	if (!config.isEnableIndirectLighting) {
		//!< IndirectLightingが有効な場合はIndirectLightPassで処理する

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

void FRenderPassDeferredLighting::PassIndirectMomentTranslate(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetTransition();
	core->SetPipeline(FRenderCoreTransition::Transition::MomentTransition, context);

	DxObject::BindBufferDesc parameter = {};
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	parameter.SetHandle("gMoment",                     config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)->GetGPUHandleUAV());
	parameter.SetHandle("gReservoirDiffuse",           config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse)->GetGPUHandleUAV());
	parameter.SetHandle("gReservoirSpecular",          config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Specular)->GetGPUHandleUAV());
	//parameter.SetHandle("gReferenceMoment",            config.buffer->GetLightingGBuffer().GetIntermediate(FLightingGBuffer::Layout::Indirect_Moment)->GetGPUHandleSRV());
	//parameter.SetHandle("gReferenceReservoirDiffuse",  config.buffer->GetLightingGBuffer().GetIntermediate(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse)->GetGPUHandleSRV());
	//parameter.SetHandle("gReferenceReservoirSpecular", config.buffer->GetLightingGBuffer().GetIntermediate(FLightingGBuffer::Layout::Indirect_Reservoir_Specular)->GetGPUHandleSRV());

	parameter.SetHandle("gVelocity", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Velocity)->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::MomentTransition, context, parameter);
	core->Dispatch(context, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)->GetSize());

}

void FRenderPassDeferredLighting::PassIndirectLight(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetPathtracing();
	core->GetContext()->SetStateObject(context->GetDxCommand());

	auto commandList = context->GetCommandList();

	//* lighting texture
	commandList->SetComputeRootDescriptorTable(0, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(1, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Specular)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(2, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Diffuse)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(3, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Atlas_Specular)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(4, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)->GetGPUHandleUAV());

	//* scene
	commandList->SetComputeRootShaderResourceView(5, config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//* deferred textures
	commandList->SetComputeRootConstantBufferView(6, config.buffer->GetIndexBufferAddress());

	//* camera
	commandList->SetComputeRootConstantBufferView(7, config.camera->GetGPUVirtualAddress());

	//* config
	commandList->SetComputeRoot32BitConstants(8, 3, &config.buffer->GetLightingGBuffer().GetConfig(), 0);

	//* light
	// Directional Light
	commandList->SetComputeRootConstantBufferView(9,  config.scene->directionalLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(10, config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(11, config.scene->directionalLightParams_->GetGPUVirtualAddress());

	// Point Light
	commandList->SetComputeRootConstantBufferView(12,  config.scene->pointLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(13, config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(14, config.scene->pointLightParams_->GetGPUVirtualAddress());

	// Spot Light
	commandList->SetComputeRootConstantBufferView(15, config.scene->spotLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(16, config.scene->spotLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(17, config.scene->spotLightParams_->GetGPUVirtualAddress());

	// Sky Light
	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		commandList->SetComputeRootConstantBufferView(18, component->GetGPUVirtualAddress());
	});
	

	core->GetContext()->DispatchRays(context->GetDxCommand(), config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)->GetSize());

}

void FRenderPassDeferredLighting::PassIndirectDenoiser(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetPathtracing();
	core->SetDenoiserPipeline(FRenderCorePathtracing::DenoiserType::EdgeStopping, context);

	config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->TransitionBeginUnordered(context);

	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	desc.SetHandle("gOutput",            config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleUAV());
	desc.SetHandle("gReservoirDiffuse",  config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Diffuse)->GetGPUHandleSRV());
	desc.SetHandle("gReservoirSpecular", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Specular)->GetGPUHandleSRV());
	desc.SetHandle("gMoment",            config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Moment)->GetGPUHandleSRV());

	//* parameter
	static const Vector3f test = { 128.0f, 0.1f, 0.1f };
	desc.Set32bitConstants("Parameter", 3, &test);

	//* deferred textures
	desc.SetAddress("gDeferredBufferIndex", config.buffer->GetIndexBufferAddress());

	core->BindDenoiserBuffer(FRenderCorePathtracing::DenoiserType::EdgeStopping, context, desc);
	core->DispatchDenoiser(context, config.buffer->GetSize());

	config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->TransitionEndUnordered(context);

}

void FRenderPassDeferredLighting::TransitionUpscaleIndirectBuffer(const DirectXQueueContext* context, const Config& config) {

	auto commandList = context->GetCommandList();

	auto core = FRenderCore::GetInstance()->GetTransition();
	

	std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {
		config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Diffuse)->TransitionBeginUnordered(),
		config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Specular)->TransitionBeginUnordered()
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	core->SetPipeline(FRenderCoreTransition::Transition::UpscaleTransition, context);

	{ //!< Diffuse
		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
		desc.SetHandle("gInput",  config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Diffuse)->GetGPUHandleSRV());
		desc.SetHandle("gOutput", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Diffuse)->GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreTransition::Transition::UpscaleTransition, context, desc);
		core->Dispatch(context, config.buffer->GetSize());
	}

	{ //!< Specular
		
		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
		desc.SetHandle("gInput",  config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir_Specular)->GetGPUHandleSRV());
		desc.SetHandle("gOutput", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Specular)->GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreTransition::Transition::UpscaleTransition, context, desc);
		core->Dispatch(context, config.buffer->GetSize());
	}

	barriers = {
		config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Diffuse)->TransitionEndUnordered(),
		config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Resolution_Specular)->TransitionEndUnordered()
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassDeferredLighting::TransitionLightingPass(const DirectXQueueContext* context, const Config& config) {

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
