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

	//* 合成処理
	// todo:

	//* 透明パス
	RenderTransparentGeometries(context);

	//* ポストプロセス
	// todo:
}

void FSceneRenderer::RenderOpaqueGeometries(const DirectXThreadContext* context) {
	context;

	const auto& geometries = scene_->GetGeometries();

	// 不透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->GetTransparency() == AGeometryActor::Transparency::Opaque) {
			// todo: render opaque
			geometry->RenderOpaque();
		}
	}

}

void FSceneRenderer::RenderTransparentGeometries(const DirectXThreadContext* context) {
	context;

	const auto& geometries = scene_->GetGeometries();

	// 半透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->GetTransparency() == AGeometryActor::Transparency::Transparent) {
			// todo: render transparent
			geometry->RenderTransparent();
		}
	};
}
