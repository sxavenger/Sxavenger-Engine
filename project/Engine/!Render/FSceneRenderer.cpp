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
	// todo:

	//* 合成処理
	// todo:

	//* 透明パス
	RenderTransparentGeometries(context);

	//* ポストプロセス
	// todo:
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
