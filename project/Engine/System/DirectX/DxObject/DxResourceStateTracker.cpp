#include "DxResourceStateTracker.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStateTracker class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ResourceStateTracker::SetName(const wchar_t* name) {
	if (resource_ != nullptr) {
		resource_->SetName(name);
	}
}

bool ResourceStateTracker::IsExpectedState(D3D12_RESOURCE_STATES state) const {
	return state == state_;
}

void ResourceStateTracker::TransitionToExpectedState(CommandContext* context, D3D12_RESOURCE_STATES state) {
	if (IsExpectedState(state)) {
		return;
	}

	// stateが期待値と異なる場合、遷移を行う.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = Get();
	barrier.Transition.StateBefore = state_;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	context->GetCommandList()->ResourceBarrier(1, &barrier);

	// 遷移後の状態を期待値として保存する.
	state_ = state;
}

DxObject::ResourceStateTracker ResourceStateTracker::CreateCommittedResource(DxObject::Device* device, D3D12_HEAP_PROPERTIES* prop, D3D12_HEAP_FLAGS flags, D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clearValue) {
	DxObject::ResourceStateTracker result;
	
	auto hr = device->GetDevice()->CreateCommittedResource(
		prop,
		flags,
		desc,
		state,
		clearValue,
		IID_PPV_ARGS(&result.resource_)
	);

	result.state_ = state;

	Exception::Assert(SUCCEEDED(hr), "resource state tracker create committed resource failed.");
	return result;
}
