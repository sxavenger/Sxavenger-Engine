#include "DxResourceTracker.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceTracker class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::unique_lock<std::mutex> ResourceTracker::Transition(const CommandContext* context, D3D12_RESOURCE_STATES state) {
	std::unique_lock<std::mutex> lock(mutex_);

	if (state_ == state) {
		return lock;
	}

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = state_;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	context->GetCommandList()->ResourceBarrier(1, &barrier);

	state_ = state;

	return lock;
}
