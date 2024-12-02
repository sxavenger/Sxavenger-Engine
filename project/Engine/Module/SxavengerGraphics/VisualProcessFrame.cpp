#include "VisualProcessFrame.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// VisualProcessFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void VisualProcessFrame::Create(const Vector2ui& size) {
	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		buffers_[i] = std::make_unique<MultiViewTexture>();
		buffers_[i]->Create(VIEWFLAG_UAV | VIEWFLAG_SRV, size, {}, kOffscreenFormat);
	}
}

void VisualProcessFrame::Term() {
}

void VisualProcessFrame::TransitionBeginProcess(const DirectXThreadContext* context) {

	std::array<D3D12_RESOURCE_BARRIER, kProcessBufferNum_> barrires = {};

	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		barrires[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrires[i].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrires[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrires[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	context->GetCommandList()->ResourceBarrier(kProcessBufferNum_, barrires.data());
}

void VisualProcessFrame::TransitonEndProcess(const DirectXThreadContext* context) {

	std::array<D3D12_RESOURCE_BARRIER, kProcessBufferNum_> barrires = {};

	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		barrires[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrires[i].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrires[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrires[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	context->GetCommandList()->ResourceBarrier(kProcessBufferNum_, barrires.data());
}

void VisualProcessFrame::NextResultBufferIndex() {
	resultBufferIndex_++;
	resultBufferIndex_ %= kProcessBufferNum_;
}

void VisualProcessFrame::ResetResultBufferIndex() {
	resultBufferIndex_ = 0;
}

