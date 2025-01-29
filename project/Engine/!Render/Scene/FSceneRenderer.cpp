#include "FSceneRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Render(const DirectXThreadContext* context) {
	if (!CheckRender()) { //!< scene or view is not set
		return;
	}

	//* ベースパス
	RenderOpaqueGeometries(context);

	//* 照明と影の処理
	ProcessLighting(context);

	/*SetupRaytracing(context);
	ProcessRaytracingReflection(context);
	context->TransitionAllocator();*/

	//* 合成処理
	// todo:

	//* 透明パス
	RenderTransparentGeometries(context);

	//* ポストプロセス
	PostProcessPass(context);
}

bool FSceneRenderer::CheckRender() const {
	if (textures_ == nullptr || scene_ == nullptr || camera_ == nullptr) {
		return false;
	}

	return true;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FSceneRenderer::GetDebugTexture() const {
	return textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleSRV();
}

void FSceneRenderer::RenderOpaqueGeometries(const DirectXThreadContext* context) {

	const auto& geometries = scene_->GetGeometries();

	textures_->BeginBasePass(context);

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

	// 不透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->CheckVisibility(AGeometryActor::Transparency::Opaque)) {
			geometry->RenderOpaque(rendererContext);
		}
	}

	textures_->EndBasePass(context);
}

void FSceneRenderer::ProcessLighting(const DirectXThreadContext* context) {

	scene_->SetupTopLevelAS(context);

	const auto& lights = scene_->GetLights();

	textures_->BeginLightingPass(context);

	ALightActor::RendererContext rendererContext = {};
	rendererContext.context        = context;
	rendererContext.size           = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());
	rendererContext.parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	rendererContext.parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Albedo_AO)->GetGPUHandleSRV());
	rendererContext.parameter.SetHandle("gNormal",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	rendererContext.parameter.SetHandle("gPosition", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleSRV());
	rendererContext.parameter.SetAddress("gScene",   scene_->GetTopLevelAS().GetGPUVirtualAddress());

	if (lights.empty()) {
		// light empty描画
		RenderEmptyLight(rendererContext);

	} else {
		// 照明の処理
		for (auto light : lights) {
			if (light->IsActive()) {
				light->Render(rendererContext);
			}
		}
	}

	textures_->EndLightingPass(context);
}

void FSceneRenderer::RenderTransparentGeometries(const DirectXThreadContext* context) {

	const auto& geometries = scene_->GetGeometries();

	textures_->BeginForward(context);

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

	// 半透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->CheckVisibility(AGeometryActor::Transparency::Transparent)) {
			geometry->RenderTransparent(rendererContext);
		}
	};

	textures_->EndForward(context);
}

void FSceneRenderer::PostProcessPass(const DirectXThreadContext* context) {
	if (textures_ == nullptr || setting_ == nullptr) {
		return;
	}

	// ポストプロセスのTextureの確認
	if (processTextures_ == nullptr || !processTextures_->CheckMatchTexture(textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result))) {
		processTextures_ = std::make_unique<FPostProcessTextures>();
		processTextures_->Create(kProcessTextureSize, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result));
	}

	if (setting_->GetProcesses().empty()) {
		return;
	}

	processTextures_->CopyFromTexture(context, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result));

	FPostProcess::ProcessContext processContext = {};
	processContext.context       = context;
	processContext.size          = textures_->GetSize();
	processContext.textures      = processTextures_.get();

	processContext.sceneTextures = textures_;

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());
	parameter.SetAddress("gConfig", textures_->GetParameter());

	processContext.parameter = parameter;

	for (auto process : setting_->GetProcesses()) {
		process->Process(processContext);
	}

	processTextures_->CopyToTexture(context, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result));
}

void FSceneRenderer::RenderEmptyLight(const ALightActor::RendererContext& context) {

	FRenderCoreLight::LightType emptyType = FRenderCoreLight::LightType::Empty;

	if (config_.isEmptyLightAlbedo) {
		emptyType = FRenderCoreLight::LightType::AlbedoEmpty;
	}

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		emptyType, context.context, context.size
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context.context);

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		emptyType, context.context, context.parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context.context);
}
