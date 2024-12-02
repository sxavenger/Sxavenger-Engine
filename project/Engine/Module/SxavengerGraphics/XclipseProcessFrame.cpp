#include "XclipseProcessFrame.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// XclipseProcessFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void XclipseProcessFrame::Create(const Vector2ui& size) {
	buffer_ = std::make_unique<MultiViewTexture>();
	buffer_->Create(VIEWFLAG_UAV, size, {}, kOffscreenFormat);
}

void XclipseProcessFrame::Term() {
}

void XclipseProcessFrame::TransitionBeginProcess(const DirectXThreadContext* context) {
	buffer_->TransitionBeginUnordered(context);
}

void XclipseProcessFrame::TransitionEndProcess(const DirectXThreadContext* context) {
	buffer_->TransitionEndUnordered(context);
}