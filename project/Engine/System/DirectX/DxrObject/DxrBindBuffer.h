#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* c++
#include <vector>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class WriteBindBufferType : bool {
	VirtualAddress,
	DescriptorHandle,
};

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct WriteBindBufferDesc {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Container = std::vector<std::optional<DxObject::GPUBuffer>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetBuffer(uint32_t index, const std::optional<DxObject::GPUBuffer>& buffer);
	void SetAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	void SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	//* getter *//

	DxObject::GPUBuffer GetBuffer(uint32_t index) const;
	D3D12_GPU_VIRTUAL_ADDRESS GetAddress(uint32_t index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandle(uint32_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Container container_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferTable class
////////////////////////////////////////////////////////////////////////////////////////////
class WriteBindBufferTable {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Table = std::vector<WriteBindBufferType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Resize(uint32_t size) { table_.resize(size); }

	void Register(uint32_t index, WriteBindBufferType type);
	void RegisterAddress(uint32_t index);
	void RegisterHandle(uint32_t index);

	//* getter *//

	WriteBindBufferDesc::Container GetWriteBuffers(const WriteBindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Table table_;
};



_DXROBJECT_NAMESPACE_END
