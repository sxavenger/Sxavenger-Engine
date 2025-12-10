#include "DxResourceDimensionBuffer.h"
DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseResourceDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

BaseResourceDimensionBuffer::~BaseResourceDimensionBuffer() {
	if (resource_ == nullptr) {
		return;
	}

	ResourceStorage::GetInstance()->Delete(resource_);
}

ID3D12Resource* BaseResourceDimensionBuffer::GetResource() const {
	Exception::Assert(resource_ != nullptr, "Dimension Buffer not create.");
	return resource_->Get();
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseResourceDimensionBuffer::GetGPUVirtualAddress() const {
	Exception::Assert(resource_ != nullptr, "Dimension Buffer not create.");
	return resource_->GetGPUVirtualAddress();
}

void BaseResourceDimensionBuffer::CreateResource(Device* device, uint32_t size) {

	// 引数の保存
	size_ = size;

	// heapの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = GetByteSize();
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// resourceの生成
	resource_ = ResourceStorage::GetInstance()->CreateCommittedResource(
		device,
		prop,
		desc,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	
}

bool BaseResourceDimensionBuffer::CheckIndex(size_t index) const {
	return index < size_;
}
