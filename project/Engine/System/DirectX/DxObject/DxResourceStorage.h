#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"

//* c++
#include <list>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class ResourceStorage;

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class ResourceBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ResourceBuffer(const ResourceStorage* storage) : storage_(storage) {}
	~ResourceBuffer() = default;

	//* resource option *//

	void CreateCommittedResource(
		Device* device,
		const D3D12_HEAP_PROPERTIES& prop,
		const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON,
		std::optional<D3D12_CLEAR_VALUE> clearValue = std::nullopt
	);

	void SetName(const std::wstring& name);

	ID3D12Resource* Get(size_t index) const;
	ID3D12Resource* Get() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const size_t kSwapBufferCount = 2;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *//

	std::array<ComPtr<ID3D12Resource>, kSwapBufferCount> buffers_;

	//* external *//

	const ResourceStorage* storage_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ResourceStorage {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	void SwapBuffer();

	//* getter *//

	size_t GetBufferIndex() const { return bufferIndex_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container *//

	std::list<std::shared_ptr<ResourceBuffer>> container_;

	//* parameter *//

	size_t bufferIndex_ = 0;

};

_DXOBJECT_NAMESPACE_END
