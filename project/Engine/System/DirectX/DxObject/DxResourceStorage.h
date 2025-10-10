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
#include <queue>

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<std::shared_ptr<ResourceBuffer>>;
	using Iterator  = Container::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Buffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void AssignAddress();

		//=========================================================================================
		// public variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource;
		std::optional<D3D12_GPU_VIRTUAL_ADDRESS> address;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ResourceBuffer() = delete;
	ResourceBuffer(const ResourceStorage* storage, const Iterator& iterator) : storage_(storage), iterator_(iterator) {}

	~ResourceBuffer() = default;

	//* resource option *//

	void SetName(const std::wstring& name);

	ID3D12Resource* Get(size_t index) const;
	ID3D12Resource* Get() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress(size_t index) const;
	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* container option *//

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const size_t kSwapBufferCount = 2;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *//

	std::array<Buffer, kSwapBufferCount> buffers_;

	//* external *//

	const ResourceStorage* storage_;
	const Iterator iterator_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateCommittedResource(
		Device* device,
		const D3D12_HEAP_PROPERTIES& prop,
		const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES state                        = D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_FLAGS flags                             = D3D12_HEAP_FLAG_NONE,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ResourceStorage {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* container option *//

	std::shared_ptr<ResourceBuffer> CreateBuffer();

	void Delete(const ResourceBuffer::Iterator& iterator);

	void Destroy();

	void SwapBuffer();

	//* getter *//

	size_t GetBufferIndex() const { return bufferIndex_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container *//

	ResourceBuffer::Container container_;

	std::queue<ResourceBuffer::Iterator> destroyQueue_;

	//* parameter *//

	size_t bufferIndex_ = 0;

};

_DXOBJECT_NAMESPACE_END
