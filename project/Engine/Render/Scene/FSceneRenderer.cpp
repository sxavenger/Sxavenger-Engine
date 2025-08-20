#include "FSceneRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"
#include "../FMainRender.h"

//* engine
//* component
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Component/Components/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/Components/Light/Rect/RectLightComponent.h>
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/Components/Particle/GPUParticleComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Config structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Sxl::Flag<FSceneRenderer::Config::Status> FSceneRenderer::Config::CheckStatus() const {
	Sxl::Flag<Status> status = Status::Success;

	if (buffer == nullptr) {
		Logger::WarningRuntime("warning | [FSceneRenderer]::Config::CheckStatus", "buffer is nullptr.");
		status |= Status::Error_Buffer;
	}

	if (scene == nullptr) {
		Logger::WarningRuntime("warning | [FSceneRenderer]::Config::CheckStatus", "scene is nullptr.");
		status |= Status::Error_Scene;
	}

	if (camera == nullptr) {
		Logger::WarningRuntime("warning | [FSceneRenderer]::Config::CheckStatus", "camera is nullptr.");
		status |= Status::Error_Camera;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Render(const DirectXQueueContext* context, const Config& _config) {
	context->RequestQueue(DirectXQueueContext::RenderQueue::Direct); //!< RenderQueueを要求

	// render configの適用, 確認
	Config config                    = ApplyConfig(_config);
	Sxl::Flag<Config::Status> status = config.CheckStatus();

	if (status.Any(Config::Status::Status_Error)) {
		Logger::ErrorRuntime("error | [FSceneRenderer]::Render", "config is invalid.");
		return;
	}

	switch (config.technique) {
		case GraphicsTechnique::Deferred:
			RenderBasePass(context, config);
			ProcessLightingPass(context, config);
			ProcessAmbientPass(context, config);
			RenderTransparentPass(context, config);
			ProcessPostProcessPass(context, config);
			ProcessTonemapPass(context, config);
			break;

		case GraphicsTechnique::Pathtracing:
			break;
	}
}

void FSceneRenderer::ResetReservoir() {
	reservoir_ = std::nullopt;
}

uint32_t FSceneRenderer::GetReservoirSampleCount() const {
	if (!reservoir_.has_value()) {
		return 0;
	}

	return reservoir_.value().GetCurrentSampleCount();
}

uint32_t FSceneRenderer::GetSamleCount() const {
	FRenderCorePathtracing::Reservoir reservoir;
	return reservoir.sampleCount;
}

void FSceneRenderer::DebugGui() {
	ImGui::DragFloat3("Test", &test_.x, 0.01f);
}

FSceneRenderer::Config FSceneRenderer::ApplyConfig(const Config& _config) const {
	Config config = _config;

	if (config.camera == nullptr) { //!< cameraが設定されていない場合, Tagのcameraを取得
		if (config.tag != CameraComponent::Tag::None) {
			// component storage から tag に一致する camera component を取得.
			sComponentStorage->ForEachActive<CameraComponent>([&](CameraComponent* component) {
				if (component->GetTag() == config.tag) {
					config.camera = component;
				}
			});
		}
	}

	if (config.scene == nullptr) { //!< sceneが設定されていない場合, main renderから取得
		config.scene = FMainRender::GetInstance()->GetScene();
	}

	return config;
}

void FSceneRenderer::RenderBasePass(const DirectXQueueContext* context, const Config& config) {

	config.buffer->BeginRenderTargetDeferred(context);

	RenderBasePassStaticMesh(context, config);
	RenderBasePassSkinnedMesh(context, config);

	config.buffer->EndRenderTargetDeferred(context);

}

void FSceneRenderer::RenderBasePassStaticMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Deferred_MeshVS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		auto mesh     = component->GetMesh();
		auto material = component->GetMaterial();

		if (material->GetMode() != UAssetMaterial::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		// メッシュの描画
		mesh->BindIABuffer(context);

		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials", material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Deferred_MeshVS, context, parameter);

		mesh->DrawCall(context, 1);

	});

}

void FSceneRenderer::RenderBasePassSkinnedMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Deferred_MeshVS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {

		auto material = component->GetMaterial();
		
		if (material->GetMode() != UAssetMaterial::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		// メッシュの描画
		component->BindIABuffer(context);

		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Deferred_MeshVS, context, parameter);

		component->DrawCall(context, 1);
	});

}

void FSceneRenderer::ProcessLightingPass(const DirectXQueueContext* context, const Config& config) {

	
	{ //* Direct Lighting
		config.buffer->BeginRenderTargetLightingDirect(context);

		ProcessLightingPassEmpty(context, config);

		//!< Punctual light
		ProcessLightingPassDirectionalLight(context, config);
		ProcessLightingPassPointLight(context, config);

		//!< Area light

		//!< Sky light
		ProcessLightingPassSkyLight(context, config);

		config.buffer->EndRenderTargetLightingDirect(context);
	}
	

	if (config.isEnableIndirectLighting) { //* Indirect Lighting
		ProcessLightingPassIndirect(context, config);
		ProcessLightingPassIndirectDenoiser(context, config);
	}
	

	LightingPassTransition(context, config);

}

void FSceneRenderer::ProcessLightingPassEmpty(const DirectXQueueContext* context, const Config& config) {

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

void FSceneRenderer::ProcessLightingPassDirectionalLight(const DirectXQueueContext* context, const Config& config) {

	uint32_t count = config.scene->directionalLightCount_->At(0);

	if (count == 0) {
		return;
	}

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Directional, context, config.buffer->GetSize()
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

	// direcitonal light parameter
	parameter.SetAddress("gTransforms", config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	parameter.SetAddress("gParameters", config.scene->directionalLightParams_->GetGPUVirtualAddress());
	parameter.SetAddress("gShadows",    config.scene->directionalLightShadowParams_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Directional, context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context, count);

}

void FSceneRenderer::ProcessLightingPassPointLight(const DirectXQueueContext* context, const Config& config) {

	uint32_t count = config.scene->pointLightCount_->At(0);

	if (count == 0) {
		return;
	}

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

	// point light parameter
	parameter.SetAddress("gTransforms", config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	parameter.SetAddress("gParameters", config.scene->pointLightParams_->GetGPUVirtualAddress());
	parameter.SetAddress("gShadows",    config.scene->pointLightShadowParams_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Point, context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context, count);

}

void FSceneRenderer::ProcessLightingPassSkyLight(const DirectXQueueContext* context, const Config& config) {
	if (config.isEnableIndirectLighting) {
		return; //!< Indirect Lightingで処理するため, スキップ
	}

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyLight, context, config.buffer->GetSize()
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

}

void FSceneRenderer::ProcessLightingPassIndirect(const DirectXQueueContext* context, const Config& config) {

	if (reservoir_.has_value()) {
		(*reservoir_).IncrimentFrame();

	} else {
		reservoir_ = FRenderCorePathtracing::Reservoir{};
	}



	config.scene->GetStateObjectContext().SetStateObject(context->GetDxCommand());

	auto commandList = context->GetCommandList();

	config.buffer->BeginUnorderedLightingIndirect(context);

	//* lighting texture
	commandList->SetComputeRootDescriptorTable(0, config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir)->GetGPUHandleUAV());

	//* scene
	commandList->SetComputeRootShaderResourceView(1, config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	//* deferred texture
	commandList->SetComputeRootDescriptorTable(2, config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(3, config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(4, config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(5, config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(6, config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());

	//* camera
	commandList->SetComputeRootConstantBufferView(7, config.camera->GetGPUVirtualAddress());

	//* reserviour
	commandList->SetComputeRoot32BitConstants(8, 3, &(reservoir_.value()), 0);

	//* light
	// Directional Light
	commandList->SetComputeRootConstantBufferView(9,  config.scene->directionalLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(10, config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(11, config.scene->directionalLightParams_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(12, config.scene->directionalLightShadowParams_->GetGPUVirtualAddress());

	// Point Light
	commandList->SetComputeRootConstantBufferView(13, config.scene->pointLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(14, config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(15, config.scene->pointLightParams_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(16, config.scene->pointLightShadowParams_->GetGPUVirtualAddress());

	// Sky Light
	std::optional<D3D12_GPU_VIRTUAL_ADDRESS> address = std::nullopt;
	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		address = component->GetGPUVirtualAddress();
	});
	commandList->SetComputeRootConstantBufferView(17, address.value_or(D3D12_GPU_VIRTUAL_ADDRESS{}));

	config.scene->GetStateObjectContext().DispatchRays(context->GetDxCommand(), config.buffer->GetSize());


	config.buffer->EndUnorderedLightingIndirect(context);

}

void FSceneRenderer::ProcessLightingPassIndirectDenoiser(const DirectXQueueContext* context, const Config& config) {

	config.buffer->BeginProcessDenoiser(context);

	auto textures = config.buffer->GetProcessTextures();

	{ //!< Edge stopping function.

		auto core = FRenderCore::GetInstance()->GetPathtracing();

		core->SetDenoiserPipeline(FRenderCorePathtracing::DenoiserType::EdgeStopping, context);

		DxObject::BindBufferDesc desc = {};
		//* common parameter
		desc.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

		//* textures
		desc.SetHandle("gOutput",   textures->GetIndexTexture()->GetGPUHandleUAV());
		desc.SetHandle("gIndirect", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect_Reservoir)->GetGPUHandleSRV());

		//* parameter
		desc.Set32bitConstants("Parameter", 3, &test_);

		//* deferred textures
		desc.SetHandle("gAlbedo",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetGPUHandleSRV());
		desc.SetHandle("gNormal",   config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetGPUHandleSRV());
		desc.SetHandle("gMaterial", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetGPUHandleSRV());
		desc.SetHandle("gPosition", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());
		desc.SetHandle("gDepth",    config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());

		core->BindDenoiserBuffer(FRenderCorePathtracing::DenoiserType::EdgeStopping, context, desc);
		core->DispatchDenoiser(context, config.buffer->GetSize());
	}

	config.buffer->EndProcessDenoiser(context);

}

void FSceneRenderer::LightingPassTransition(const DirectXQueueContext* context, const Config& config) {

	config.buffer->BeginUnorderedMainScene(context);

	auto core = FRenderCore::GetInstance()->GetTransition();

	core->SetPipeline(FRenderCoreTransition::Transition::LightingTransition, context);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// input
	parameter.SetHandle("gDirect",   config.buffer->GetGBuffer(FLightingGBuffer::Layout::Direct)->GetGPUHandleSRV());
	parameter.SetHandle("gIndirect", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Indirect)->GetGPUHandleSRV());

	// output
	parameter.SetHandle("gOutput", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::LightingTransition, context, parameter);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->EndUnorderedMainScene(context);
	
}

void FSceneRenderer::ProcessAmbientPass(const DirectXQueueContext* context, const Config& config) {
	ProcessAmbientPassSkyBox(context, config);
}

void FSceneRenderer::ProcessAmbientPassSkyBox(const DirectXQueueContext* context, const Config& config) {

	config.buffer->BeginUnorderedMainScene(context);

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::CompositeType::Environment, context
	);

	DxObject::BindBufferDesc parameter = {};
	// common
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	// deferred
	parameter.SetHandle("gDepth", config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		if (!component->IsEnableEnvironment()) {
			return; //!< Environmentが設定されていない場合はスキップ
		}

		// environment
		parameter.SetAddress("gParameter", component->GetGPUVirtualAddress());

		// output
		parameter.SetHandle("gOutput", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
			FRenderCoreProcess::CompositeType::Environment, context, parameter
		);

		FRenderCore::GetInstance()->GetProcess()->Dispatch(context, config.buffer->GetSize());
	});

	config.buffer->EndUnorderedMainScene(context);

}

void FSceneRenderer::RenderTransparentPass(const DirectXQueueContext* context, const Config& config) {

	config.buffer->BeginRenderTargetMainTransparent(context);

	RenderTransparentPassStaticMesh(context, config);
	RenderTransparentPassParticle(context, config);

	config.buffer->EndRenderTargetMainTransparent(context);

}

void FSceneRenderer::RenderTransparentPassStaticMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Forward_MeshVS, context, config.buffer->GetSize());

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	// componentを取得
	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		auto mesh     = component->GetMesh();
		auto material = component->GetMaterial();

		if (material->GetMode() == UAssetMaterial::Mode::Opaque) {
			return;
		}

		// メッシュの描画
		mesh->BindIABuffer(context);

		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Forward_MeshVS, context, parameter);


		mesh->DrawCall(context, 1);
	});
}

void FSceneRenderer::RenderTransparentPassParticle(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();

	// componentを取得
	sComponentStorage->ForEachActive<ParticleComponent>([&](ParticleComponent* component) {
		component->DrawParticle(context, config.camera);
		// todo: rendererで描画
	});

	core->SetPipeline(FRenderCoreGeometry::Type::Forward_GPUParticleVS, context, config.buffer->GetSize());

	sComponentStorage->ForEachActive<GPUParticleComponent>([&](GPUParticleComponent* component) {
		if (!component->HasPritimive()) {
			return;
		}

		component->BindIABuffer(context);

		DxObject::BindBufferDesc parameter = {};
		//* common
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

		//* particle
		parameter.SetAddress("gConfig",    component->GetConfig().GetGPUVirtualAddress());
		parameter.SetAddress("gParticles", component->GetGPUVirtualAddress());

		//* texture
		parameter.SetHandle("gDirect", config.buffer->GetGBuffer(FLightingGBuffer::Layout::Direct)->GetGPUHandleSRV());

		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Forward_GPUParticleVS, context, parameter);

		component->DrawCall(context);
	});
}

void FSceneRenderer::ProcessPostProcessPass(const DirectXQueueContext* context, const Config& config) {
	if (!config.isEnablePostProcess) {
		return;
	}

	config.buffer->BeginPostProcess(context);

	BasePostProcess::ProcessInfo info = {};
	info.buffer = config.buffer;
	info.camera = config.camera;

	//!< Global PostProcessの処理
	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Global) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		component->Process(context, info);
	});

	// Volume PostProcessの処理
	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Volume) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		auto transform = component->GetTransform();

		if (transform == nullptr) {
			Logger::WarningRuntime("warning | [FSceneRenderer]::ProcessPostProcessPass [Volume]", "PostProcessLayerComponent [Volume] has no transform.");
			return; //!< Transformがない場合は処理しない
		}

		BasePostProcess::ProcessInfo parameter = info;
		parameter.weight = component->CalculateVolumeWeight(config.camera->GetPosition());

		if (parameter.weight <= 0.0f) {
			return; //!< 重みが0以下の場合は処理しない
		}

		component->Process(context, parameter);
	});

	//!< Local Post Processの処理
	if (auto component = config.camera->GetBehaviour()->GetComponent<PostProcessLayerComponent>()) {
		if (component->GetTag() == PostProcessLayerComponent::Tag::Local) {
			component->Process(context, info);
		}
	}


	config.buffer->EndPostProcess(context);

}

void FSceneRenderer::ProcessTonemapPass(const DirectXQueueContext* context, const Config& config) {
	if (!config.isElableTonemap) {
		return;
	}

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::CompositeType::Tonemap, context
	);

	DxObject::BindBufferDesc parameter = {};
	// common
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	parameter.SetHandle("gTexture", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

	parameter.Set32bitConstants("ColorSpaceBuffer", 1, &config.colorSpace);

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
		FRenderCoreProcess::CompositeType::Tonemap, context, parameter
	);

	FRenderCore::GetInstance()->GetProcess()->Dispatch(context, config.buffer->GetSize());

	config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->BarrierUAV(context);

}
