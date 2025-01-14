#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* c++
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class WriteBindBufferType {
	VirtualAddress,
	DescriptorHandle,
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

//! [unordered_map]
//! key:   bufferName
//! value: buffer
using BufferContainer = std::unordered_map<std::string, DxObject::GPUBuffer>;

//! [vector]
using WriteBufferContainer = std::vector<DxObject::GPUBuffer>;

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferDesc class
////////////////////////////////////////////////////////////////////////////////////////////
struct WriteBindBufferDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* container option *//

	void Clear();

	void SetBuffer(const std::string& name, const DxObject::GPUBuffer& buffer);
	void SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	void SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	//* getter *//

	const BufferContainer& GetTable() const { return container_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//! [unordered_map]
	//! key:   bufferName
	//! value: buffer
	BufferContainer container_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferTable class
////////////////////////////////////////////////////////////////////////////////////////////
class WriteBindBufferTable {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	WriteBindBufferTable()  = default;
	~WriteBindBufferTable() = default;

	void Register(const std::string& name, UINT index, WriteBindBufferType type);
	void RegisterAddress(const std::string& name, UINT index);
	void RegisterHandle(const std::string& name, UINT index);

	//* getter *//

	WriteBufferContainer GetWriteBuffer(const WriteBindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<WriteBindBufferType>      table_;
	std::unordered_map<std::string, UINT> tableIndex_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AutoResize(UINT index);

};

_DXROBJECT_NAMESPACE_END
