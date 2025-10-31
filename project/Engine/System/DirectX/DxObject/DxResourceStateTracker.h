#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"

//* lib
#include <Lib/Traits.h>

//* c++
#include <list>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStateTracker class
////////////////////////////////////////////////////////////////////////////////////////////
class ResourceStateTracker {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//_NODISCARD std::unique_lock<std::mutex> Transition(const CommandContext* context, D3D12_RESOURCE_STATES state);

	void SetName(const wchar_t* name);

	bool IsExpectedState(D3D12_RESOURCE_STATES state) const;

	void Transition(const CommandContext* context, D3D12_RESOURCE_STATES state);

	ID3D12Resource* Get() const { return resource_.Get(); }

	static ResourceStateTracker CreateCommittedResource(
		Device* device,
		D3D12_HEAP_PROPERTIES* prop,
		D3D12_HEAP_FLAGS flags,
		D3D12_RESOURCE_DESC* desc,
		D3D12_RESOURCE_STATES state,
		D3D12_CLEAR_VALUE* clearValue
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12Resource> resource_;
	D3D12_RESOURCE_STATES state_;

};

_DXOBJECT_NAMESPACE_END
