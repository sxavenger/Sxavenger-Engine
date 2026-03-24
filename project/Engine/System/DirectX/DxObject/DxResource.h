#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxDescriptor.h"

//* lib
#include <Lib/CXXAttribute.h>

//* c++
#include <optional>
#include <vector>

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

	Resource() = default;

	//* resource option *//

	void CreateCommitted(
		DxObject::Device* device,
		const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

	void Reset();

	void SetName(const std::wstring& name) const;

	void Map(void** data, const std::optional<D3D12_RANGE>& range = std::nullopt);

	void Unmap(const std::optional<D3D12_RANGE>& range = std::nullopt);

	//* transition options *//

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransition(D3D12_RESOURCE_STATES state);

	void SetTransition(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state);

	void Transition(
		DxObject::CommandContext* context,
		D3D12_RESOURCE_STATES state
	);

	//* getter *//

	ID3D12Resource* Get() const { return resource_.Get(); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	D3D12_RESOURCE_STATES GetCurrentState() const { return current_; }

	const D3D12_RESOURCE_DESC GetDesc() const;

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

	//* operator [copy] (delete) *//

	Resource(const Resource&)            = delete;
	Resource& operator=(const Resource&) = delete;

	//* operator [move] *//

	Resource(Resource&&) noexcept            = default;
	Resource& operator=(Resource&&) noexcept = default;

	//* operator [assign] *//

	Resource(std::nullopt_t) noexcept { Reset(); }
	Resource& operator=(std::nullptr_t) noexcept { Reset(); return *this; }

	//* operator [equal] *//

	bool operator==(const Resource& rhs) const { return resource_.Get() == rhs.resource_.Get(); }
	bool operator!=(const Resource& rhs) const { return resource_.Get() != rhs.resource_.Get(); }
	bool operator==(std::nullptr_t) const { return resource_.Get() == nullptr; }
	bool operator!=(std::nullptr_t) const { return resource_.Get() != nullptr; }

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
