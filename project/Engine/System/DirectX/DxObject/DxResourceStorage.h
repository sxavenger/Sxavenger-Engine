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
DXOBJECT_NAMESPACE_BEGIN

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
	ResourceBuffer(const Iterator& iterator) : iterator_(iterator) {}

	~ResourceBuffer() = default;

	//* resource option *//

	void SetName(const std::wstring& name);

	//* getter *//

	ID3D12Resource* Get(size_t index) const;
	ID3D12Resource* Get() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress(size_t index) const;
	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

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

	//-----------------------------------------------------------------------------------------
	// friend
	//-----------------------------------------------------------------------------------------
	friend class ResourceStorage;

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

	std::shared_ptr<ResourceBuffer> CreateCommittedResource(
		Device* device,
		const D3D12_HEAP_PROPERTIES& prop,
		const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES state                        = D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_FLAGS flags                             = D3D12_HEAP_FLAG_NONE,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

	void Delete(const std::shared_ptr<ResourceBuffer>& resource);

	void Destroy();

	void SwapBuffer();

	//* getter *//

	size_t GetBackBufferIndex() const { return backBufferIndex_; }

	static ResourceStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container *//

	ResourceBuffer::Container container_;

	std::queue<ResourceBuffer::Iterator> destroyQueue_;

	//* parameter *//

	size_t backBufferIndex_ = 0;
	//!< front buffer: GPU使用中(書き込み不可)
	//!< back buffer:  書き込み可能

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::shared_ptr<ResourceBuffer> RegisterResource();

};

DXOBJECT_NAMESPACE_END
