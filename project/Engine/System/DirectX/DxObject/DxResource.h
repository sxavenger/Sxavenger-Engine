#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxDescriptor.h"

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Resource class
////////////////////////////////////////////////////////////////////////////////////////////
class Resource {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Resource()                    = default;
	Resource(Resource&&) noexcept = default;

	//* resource option *//

	void CreateCommitted(
		DxObject::Device* device,
		const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

	void Reset();

	void Transition(
		DxObject::CommandContext* context,
		D3D12_RESOURCE_STATES state
	);

	void SetName(const std::wstring& name) const;

	//* getter *//

	ID3D12Resource* Get() const { return resource_.Get(); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//* helper create methods *//

	static Resource CreateBuffer(
		DxObject::Device* device,
		D3D12_HEAP_TYPE type, size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
	);

	static Resource CreateTexture(
		DxObject::Device* device,
		D3D12_RESOURCE_DIMENSION dimension, const Vector3ui& size, UINT16 miplevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue
	);

	//* delete [copy] *//
	Resource(const Resource&)            = delete;
	Resource& operator=(const Resource&) = delete;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	//* state tracker *//

	D3D12_RESOURCE_STATES current_ = D3D12_RESOURCE_STATE_COMMON;

};

DXOBJECT_NAMESPACE_END
