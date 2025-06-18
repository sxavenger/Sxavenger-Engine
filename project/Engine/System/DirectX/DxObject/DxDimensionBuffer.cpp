#include "DxDimensionBuffer.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDimensionBuffer::Release() {
	if (resource_ != nullptr) {
		resource_->Unmap(0, nullptr);
		resource_.Reset();
		address_ = std::nullopt;
	}

	size_ = NULL;
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseDimensionBuffer::GetGPUVirtualAddress() const {
	Exception::Assert(address_.has_value(), "Dimension Buffer not create.");
	return address_.value();
}

void BaseDimensionBuffer::Create(Device* devices, uint32_t size) {

	// 引数の保存
	size_ = size;

	// deviceの取り出し
	auto device = devices->GetDevice();

	// resourceの生成
	resource_ = CreateBufferResource(
		device,
		size_ * stride_
	);

	UpdateAddress();
}

void BaseDimensionBuffer::UpdateAddress() {
	address_ = (resource_ != nullptr) ? std::optional<D3D12_GPU_VIRTUAL_ADDRESS>{ resource_->GetGPUVirtualAddress() } : std::nullopt;
}

bool BaseDimensionBuffer::CheckIndex(size_t index) const {
	return index < size_;
}

////////////////////////////////////////////////////////////////////////////////////////////
// IndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

const UINT IndexDimensionBuffer::GetIndexCount() const {
	return size_;
}

const D3D12_INDEX_BUFFER_VIEW IndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW result = {};
	result.Format         = DXGI_FORMAT_R32_UINT;
	result.BufferLocation = GetGPUVirtualAddress();
	result.SizeInBytes    = static_cast<UINT>(stride_ * size_);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LineIndexDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

const UINT LineIndexDimensionBuffer::GetIndexCount() const {
	return size_ * 2;
}

const D3D12_INDEX_BUFFER_VIEW LineIndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW result = {};
	result.Format         = DXGI_FORMAT_R32_UINT;
	result.BufferLocation = GetGPUVirtualAddress();
	result.SizeInBytes    = static_cast<UINT>(stride_ * size_);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

const UINT TriangleIndexDimensionBuffer::GetIndexCount() const {
	return size_ * 3;
}

const D3D12_INDEX_BUFFER_VIEW TriangleIndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW result = {};
	result.Format         = DXGI_FORMAT_R32_UINT;
	result.BufferLocation = GetGPUVirtualAddress();
	result.SizeInBytes    = static_cast<UINT>(stride_ * size_);
	return result;
}
