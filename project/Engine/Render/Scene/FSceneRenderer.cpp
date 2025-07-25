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
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>

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

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		if (component->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		component->GetMesh()->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, config.buffer->GetSize()
		);

		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  component->GetMaterial()->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		component->GetMesh()->DrawCall(context, 1);

	});

}

void FSceneRenderer::RenderBasePassSkinnedMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {
		
		if (component->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		component->BindIABuffer(context);

		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, config.buffer->GetSize()
		);

		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  component->GetMaterial()->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		component->DrawCall(context, 1);
	});

}

void FSceneRenderer::ProcessLightingPass(const DirectXQueueContext* context, const Config& config) {

	//* Direct Lighting
	config.buffer->BeginRenderTargetLightingDirect(context);

	ProcessLightingPassEmpty(context, config);

	//!< Punctual light
	ProcessLightingPassDirectionalLight(context, config);
	ProcessLightingPassPointLight(context, config);

	//!< Area light

	//!< Sky light
	ProcessLightingPassSkyLight(context, config);

	config.buffer->EndRenderTargetLightingDirect(context);

	//* Indirect Lighting
	// todo

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

		// sky light parameter
		parameter.SetAddress("gDiffuseParameter",  component->GetDiffuseParameterBufferAddress());
		parameter.SetAddress("gSpecularParameter", component->GetSpecularParameterBufferAddress());
		parameter.SetAddress("gParameter",         component->GetParameterBufferAddress());

		parameter.SetHandle("gEnvironment",        component->GetEnvironment().value()); //!< DEBUG

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::SkyLight, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});

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
		FRenderCoreProcess::ProcessType::Environment, context
	);

	DxObject::BindBufferDesc parameter = {};
	// common
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	// deferred
	parameter.SetHandle("gDepth", config.buffer->GetDepth()->GetRasterizerGPUHandleSRV());

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		if (!component->GetEnvironment().has_value()) {
			return;
		}

		// environment
		parameter.SetAddress("gParameter", component->GetParameterBufferAddress());
		parameter.SetHandle("gEnvironment", (*component->GetEnvironment()));

		// output
		parameter.SetHandle("gOutput", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
			FRenderCoreProcess::ProcessType::Environment, context, parameter
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

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	// componentを取得
	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		if (component->GetMaterial()->GetMode() != Material::Mode::Translucent) {
			return;
		}

		component->GetMesh()->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, config.buffer->GetSize()
		);

		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  component->GetMaterial()->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		component->GetMesh()->DrawCall(context, 1);
	});
}

void FSceneRenderer::RenderTransparentPassParticle(const DirectXQueueContext* context, const Config& config) {

	// componentを取得
	sComponentStorage->ForEachActive<ParticleComponent>([&](ParticleComponent* component) {
		component->DrawParticle(context, config.camera);
	});

}

void FSceneRenderer::ProcessPostProcessPass(const DirectXQueueContext* context, const Config& config) {
	if (!config.isEnablePostProcess) {
		return;
	}

	config.buffer->BeginPostProcess(context);

	//!< Global PostProcessの処理
	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Global) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		component->Process(context, config.buffer, config.camera);
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

		if (!component->IsInsideVolume(config.camera->GetPosition())) {
			return; // TODO: 範囲外の場合, 減衰処理を行う
		}

		component->Process(context, config.buffer, config.camera);
	});

	//!< Local Post Processの処理
	if (auto component = config.camera->GetBehaviour()->GetComponent<PostProcessLayerComponent>()) {
		if (component->GetTag() == PostProcessLayerComponent::Tag::Local) {
			component->Process(context, config.buffer, config.camera);
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

}
