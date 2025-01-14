#include "DxrBindBuffer.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferDesc class
////////////////////////////////////////////////////////////////////////////////////////////

void WriteBindBufferDesc::Clear() {
	container_.clear();
}

void WriteBindBufferDesc::SetBuffer(const std::string& name, const DxObject::GPUBuffer& buffer) {
	container_[name] = buffer;
}

void WriteBindBufferDesc::SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	SetBuffer(name, address);
}

void WriteBindBufferDesc::SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	SetBuffer(name, handle);
}

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferTable class methods
////////////////////////////////////////////////////////////////////////////////////////////

void WriteBindBufferTable::Register(const std::string& name, UINT index, WriteBindBufferType type) {
	AutoResize(index);
	table_[index]     = type;
	tableIndex_[name] = index;
}

void WriteBindBufferTable::RegisterAddress(const std::string& name, UINT index) {
	Register(name, index, WriteBindBufferType::VirtualAddress);
}

void WriteBindBufferTable::RegisterHandle(const std::string& name, UINT index) {
	Register(name, index, WriteBindBufferType::DescriptorHandle);
}

WriteBufferContainer WriteBindBufferTable::GetWriteBuffer(const WriteBindBufferDesc& desc) const {

	WriteBufferContainer container = {};
	container.resize(table_.size());

	for (const auto& [name, index] : tableIndex_) {
		Assert(desc.GetTable().contains(name), "buffer is not found.");
		container[index] = desc.GetTable().at(name);
	}

	return container;
}

void WriteBindBufferTable::AutoResize(UINT index) {
	if (table_.size() <= index) {
		table_.resize(index + 1);
	}
}
