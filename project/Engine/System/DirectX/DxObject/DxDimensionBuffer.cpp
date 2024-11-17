#include "DxDimensionBuffer.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

const D3D12_GPU_VIRTUAL_ADDRESS& BaseDimensionBuffer::GetGPUVirtualAddress() const {
	Assert(address_.has_value(), "Dimension Buffer not create.");
	return address_.value();
}

void BaseDimensionBuffer::Create(Device* devices, uint32_t size, size_t stride) {

	// 引数の保存
	size_   = size;
	stride_ = static_cast<uint32_t>(stride);

	// deviceの取り出し
	auto device = devices->GetDevice();

	// resourceの生成
	resource_ = CreateBufferResource(
		device,
		size_ * stride_
	);

	address_ = resource_->GetGPUVirtualAddress();
}

void BaseDimensionBuffer::Term() {
	size_   = NULL;
	stride_ = NULL;

	address_ = std::nullopt;
}

bool BaseDimensionBuffer::CheckIndex(uint32_t index) {
	if (index < size_) {
		return false;
	}

	return true;
}

const D3D12_INDEX_BUFFER_VIEW IndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW result = {};
	result.Format         = DXGI_FORMAT_R32_UINT;
	result.BufferLocation = GetGPUVirtualAddress();
	result.SizeInBytes    = stride_ * size_;

	return result;
}
