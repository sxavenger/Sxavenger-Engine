#include "FBaseTexture.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Color4.h>

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

void FBaseTexture::TransitionBeginRenderTarget(const DirectXQueueContext* context) const {
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

void FBaseTexture::TransitionEndRenderTarget(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndRenderTarget();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FBaseTexture::ClearRenderTarget(const DirectXQueueContext* context) const {
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

void FBaseTexture::TransitionBeginUnordered(const DirectXQueueContext* context) const {
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

void FBaseTexture::TransitionEndUnordered(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginState(D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultState_;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionBeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginState(state);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionEndState(D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter  = kDefaultState_;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionEndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndState(state);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}
