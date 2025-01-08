#include "FSceneRenderer.h"



////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Render(const DirectXThreadContext* context) {
	if (scene_ == nullptr || view_ == nullptr) { //!< scene or view is not set
		return;
	}

	RenderOpaque(context);
	RenderTransparent(context);
}

void FSceneRenderer::RenderOpaque(const DirectXThreadContext* context) {
	context;

	const auto& geometries = scene_->GetGeometries();

	// 不透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->GetTransparency() == AGeometryActor::Transparency::Opaque) {
			// todo: render opaque
			geometry->Render();
		}
	}

}

void FSceneRenderer::RenderTransparent(const DirectXThreadContext* context) {
	context;

	const auto& geometries = scene_->GetGeometries();

	// 半透明なジオメトリを描画
	for (auto geometry : geometries) {
		if (geometry->GetTransparency() == AGeometryActor::Transparency::Transparent) {
			// todo: render transparent
			geometry->Render();
		}
	};
}
