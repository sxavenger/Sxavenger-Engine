#include "FSceneRenderer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::CreateTextures(const Vector2ui& size) {
	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(size);
}

void FSceneRenderer::Render(const DirectXThreadContext* context) {
	if (scene_ == nullptr || camera_ == nullptr) { //!< scene or view is not set
		return;
	}

	//* ベースパス
	RenderOpaqueGeometries(context);

	//* 照明と影の処理
	ProcessLighting(context);

	//* 合成処理
	// todo:

	//* 透明パス
	RenderTransparentGeometries(context);

	//* ポストプロセス
	// todo:
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FSceneRenderer::GetDebugTexture() const {
	return textures_->GetGBuffer(FSceneTextures::GBufferLayout::Lighting)->GetGPUHandleSRV();
}

void FSceneRenderer::RenderOpaqueGeometries(const DirectXThreadContext* context) {

	const auto& geometries = scene_->GetGeometries();

	textures_->BeginBasePass(context);

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();
	rendererContext.camera  = camera_;

	// 不透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->CheckVisibility(AGeometryActor::Transparency::Opaque)) {
			geometry->RenderOpaque(rendererContext);
		}
	}

	textures_->EndBasePass(context);
}

void FSceneRenderer::ProcessLighting(const DirectXThreadContext* context) {

	const auto& lights = scene_->GetLights();

	textures_->BeginLightingPass(context);

	ALightActor::RendererContext rendererContext = {};
	rendererContext.context        = context;
	rendererContext.size           = textures_->GetSize();

	rendererContext.parameter.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());
	rendererContext.parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	rendererContext.parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Albedo_AO)->GetGPUHandleUAV());
	rendererContext.parameter.SetHandle("gNormal",   textures_->GetGBuffer(FSceneTextures::GBufferLayout::Normal)->GetGPUHandleUAV());
	rendererContext.parameter.SetHandle("gPosition", textures_->GetGBuffer(FSceneTextures::GBufferLayout::Position)->GetGPUHandleUAV());

	// 照明の処理
	for (auto light : lights) {
		if (light->IsActive()) {
			light->Render(rendererContext);
		}
	}

	textures_->EndLightingPass(context);
}

void FSceneRenderer::RenderTransparentGeometries(const DirectXThreadContext* context) {
	context;

	const auto& geometries = scene_->GetGeometries();

	AGeometryActor::RendererContext rendererContext = {};
	rendererContext.context = context;
	rendererContext.size    = textures_->GetSize();
	rendererContext.camera  = camera_;

	// 半透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->CheckVisibility(AGeometryActor::Transparency::Transparent)) {
			geometry->RenderTransparent(rendererContext);
		}
	};
}
