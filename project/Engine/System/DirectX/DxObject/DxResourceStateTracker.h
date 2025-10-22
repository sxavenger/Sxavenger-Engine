#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStateTracker class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief リソースの状態を管理するクラス.
class ResourceStateTracker {
	//!< 廃止予定
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* resource option *//

	ID3D12Resource* Get() const { return resource_.Get(); }

	void SetName(const wchar_t* name);

	//* state tracker option *//

	bool IsExpectedState(D3D12_RESOURCE_STATES state) const;

	void TransitionToExpectedState(CommandContext* context, D3D12_RESOURCE_STATES state);

	//* helper option *//

	static DxObject::ResourceStateTracker CreateCommittedResource(
		DxObject::Device* device,
		D3D12_HEAP_PROPERTIES* prop,
		D3D12_HEAP_FLAGS flags,
		D3D12_RESOURCE_DESC* desc,
		D3D12_RESOURCE_STATES state,
		D3D12_CLEAR_VALUE* clearValue = nullptr
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12Resource> resource_;
	D3D12_RESOURCE_STATES state_;

};

_DXOBJECT_NAMESPACE_END

