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
	auto status = CheckStatus();

	if (status.Any(Status::Status_Error)) {
		return;
	}

	//* ベースパス
	if (!status.Test(Status::Warning_Scene)) {
		RenderOpaqueGeometries(context);
	}

	//* 照明と影の処理
	if (!status.Test(Status::Warning_Scene)) {
		ProcessLighting(context);
	}

	//* 合成処理
	if (!status.Test(Status::Warning_AmbientProcess)) {
		ProcessAmbientPass(context);
	}

	//* 透明パス
	if (!status.Test(Status::Warning_Scene)) {
		RenderTransparentGeometries(context);
	}

	//* ポストプロセス
	if (!status.Test(Status::Warning_PostProcess)) {
		PostProcessPass(context);
	}
}

Sxl::Flag<FSceneRenderer::Status> FSceneRenderer::CheckStatus() const {
	Sxl::Flag<Status> result = Status::Success;

	if (textures_ == nullptr) {
		result |= Status::Error_Textures;
	}

	if (camera_ == nullptr) {
		result |= Status::Error_Camera;
	}

	if (scene_ == nullptr) {
		result |= Status::Warning_Scene;
	}

	if (ambientProcesses_ == nullptr) {
		result |= Status::Warning_AmbientProcess;
	}

	if (postProcesses_ == nullptr) {
		result |= Status::Warning_PostProcess;
	}

	return result;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FSceneRenderer::GetDebugTexture() const {
	return textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleSRV();
}

void FSceneRenderer::RenderOpaqueGeometries(const DirectXThreadContext* context) {

	const auto& geometries = scene_->GetGeometries();

	textures_->BeginOpaqueBasePass(context);

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();

	rendererContext.target = MaterialComponent::BlendMode::Opaque;

	rendererContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

	// 不透明なジオメトリを描画
	RenderOpaqueGeometriesContainer(geometries, rendererContext);

	textures_->EndOpaqueBasePass(context);
}

void FSceneRenderer::ProcessLighting(const DirectXThreadContext* context) {

	//scene_->SetupTopLevelAS(context);

	const auto& lights = scene_->GetLights();

	textures_->BeginLightingPass(context);

	ALightActor::RendererContext rendererContext = {};
	rendererContext.context        = context;
	rendererContext.size           = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());
	rendererContext.parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	rendererContext.parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
	rendererContext.parameter.SetHandle("gNormal",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	rendererContext.parameter.SetHandle("gPosition", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleSRV());
	rendererContext.parameter.SetHandle("gMaterial", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Material_AO)->GetGPUHandleSRV());
	rendererContext.parameter.SetAddress("gScene",   scene_->GetTopLevelAS().GetGPUVirtualAddress());

	if (lights.empty()) {
		// light empty描画
		RenderEmptyLight(rendererContext);

	} else {
		// 照明の処理
		for (auto light : lights | std::views::filter([](ALightActor* light) { return light->IsActive(); })) {
			light->Render(rendererContext);
		}
	}

	textures_->EndLightingPass(context);
}

void FSceneRenderer::ProcessAmbientPass(const DirectXThreadContext* context) {
	if (!ambientProcesses_->CheckProcess()) {
		return;
	}

	FAmbientProcess::ProcessContext processContext = {};
	processContext.context  = context;
	processContext.size     = textures_->GetSize();
	processContext.textures = textures_;

	processContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());
	processContext.parameter.SetAddress("gConfig", textures_->GetParameter());
	processContext.parameter.SetHandle("gDepth", textures_->GetDepth()->GetRasterizerGPUHandleSRV());

	ambientProcesses_->ExecuteProcess(processContext);
}

void FSceneRenderer::RenderTransparentGeometries(const DirectXThreadContext* context) {

	const auto& geometries = scene_->GetGeometries();

	textures_->BeginTransparentBasePass(context);

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();

	rendererContext.target = MaterialComponent::BlendMode::Translucent;

	rendererContext.parameter.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

	// 半透明なジオメトリを描画
	RenderTransparentGeometriesContainer(geometries, rendererContext);

	textures_->EndTransparentBasePass(context);
}

void FSceneRenderer::PostProcessPass(const DirectXThreadContext* context) {
	// ポストプロセスのTextureの確認
	if (processTextures_ == nullptr || !processTextures_->CheckMatchTexture(textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main))) {
		processTextures_ = std::make_unique<FPostProcessTextures>();
		processTextures_->Create(kProcessTextureSize, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main));
	}

	if (!postProcesses_->CheckProcess()) {
		return;
	}

	processTextures_->CopyFromTexture(context, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main));

	textures_->BeginPostProcessPass(context);

	FPostProcess::ProcessContext processContext = {};
	processContext.context       = context;
	processContext.size          = textures_->GetSize();
	processContext.textures      = processTextures_.get();

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());
	parameter.SetAddress("gConfig",  textures_->GetParameter());
	parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleSRV());

	processContext.parameter = parameter;

	postProcesses_->ExecuteProcess(processContext);

	textures_->EndPostProcessPass(context);

	processTextures_->CopyToTexture(context, textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main));
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

void FSceneRenderer::RenderOpaqueGeometriesContainer(
	const AGeometryActor::Container& container, const AGeometryActor::RendererContext& context) {
	
	for (auto geometry : container | std::views::filter([](AGeometryActor* actor) { return actor->IsActive(); })) {
		geometry->Render(context);
		RenderOpaqueGeometriesContainer(geometry->GetChildren(), context);
	}
}

void FSceneRenderer::RenderTransparentGeometriesContainer(
	const AGeometryActor::Container& container, const AGeometryActor::RendererContext& context) {

	for (auto geometry : container | std::views::filter([](AGeometryActor* actor) { return actor->IsActive(); })) {
		geometry->Render(context);
		RenderTransparentGeometriesContainer(geometry->GetChildren(), context);
	}
}
