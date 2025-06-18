#include "DxVectorDimensionBuffer.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseVectorDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseVectorDimensionBuffer::Release() {
	if (resource_ != nullptr) {
		resource_->Unmap(0, nullptr);
		resource_.Reset();
	}

	size_     = NULL;
	capacity_ = NULL;

	address_ = std::nullopt;
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseVectorDimensionBuffer::GetGPUVirtualAddress() const {
	Exception::Assert(address_.has_value(), "Vector Dimension Buffer not create.");
	return address_.value();
}

void BaseVectorDimensionBuffer::UpdateAddress() {
	address_ = (resource_ != nullptr) ? std::optional<D3D12_GPU_VIRTUAL_ADDRESS>{ resource_->GetGPUVirtualAddress() } : std::nullopt;
}

void BaseVectorDimensionBuffer::Create(Device* device, uint32_t capacity) {
	// 引数の保存
	capacity_ = capacity;

	// resourceの生成
	resource_ = CreateBufferResource(
		device->GetDevice(),
		capacity_ * stride_
	);
	resource_->SetName(L"vector dimension buffer");

	UpdateAddress();
}

bool BaseVectorDimensionBuffer::CheckIndex(size_t index) const {
	return index < size_;
}
