#include "FCanvas.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FCanvas class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FCanvas::Render(const DirectXThreadContext* context) {
	if (!CheckRender()) {
		return;
	}

	LLayer::RenderContext renderContext = {};
	renderContext.context = context;
	renderContext.size    = textures_->GetSize();

	renderContext.parameter.SetAddress("gParameter", textures_->GetParameter());

	textures_->BeginTransparentBasePass(context);

	for (auto& layer : layers_) {
		if (layer->CheckVisibility()) {
			layer->Render(renderContext);
		}
	}

	textures_->EndTransparentBasePass(context);
}

bool FCanvas::CheckRender() const {

	if (textures_ == nullptr) {
		return false;
	}

	return true;
}
