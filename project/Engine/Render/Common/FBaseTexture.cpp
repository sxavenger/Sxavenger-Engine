#include "FBaseTexture.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseTexture::Term() {
	resource_.Reset();
	descriptorSRV_.Delete();
	descriptorRTV_.Delete();
	descriptorUAV_.Delete();
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginRenderTarget() const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultState_;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionBeginRenderTarget(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginRenderTarget();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionEndRenderTarget() const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = kDefaultState_;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionEndRenderTarget(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndRenderTarget();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FBaseTexture::ClearRenderTarget(const DirectXThreadContext* context) const {
	static constexpr Color4f clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	context->GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		&clearColor.r,
		0, nullptr
	);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginUnordered() const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultState_;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionBeginUnordered(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionEndUnordered() const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = kDefaultState_;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionEndUnordered(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}
