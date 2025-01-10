#include "AdaptiveRenderFrame.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// AdaptiveRenderFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AdaptiveRenderFrame::Create(const Vector2ui& size) {
	buffer_ = std::make_unique<MultiViewTextureBuffer>();
	buffer_->Create(VIEWFLAG_RENDER_TEXTURE, size, {}, kOffscreenFormat);
}

void AdaptiveRenderFrame::Term() {
}

void AdaptiveRenderFrame::TransitionBeginRenderTarget(const DirectXThreadContext* context) {
	buffer_->TransitionBeginRenderTarget(context);
}

void AdaptiveRenderFrame::TransitionEndRenderTarget(const DirectXThreadContext* context) {
	buffer_->TransitionEndRenderTarget(context);
}

void AdaptiveRenderFrame::ClearRenderTarget(const DirectXThreadContext* context) {
	buffer_->ClearRenderTarget(context);
}
