#include "DxAppendConsumeBuffer.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAppendConsumeBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAppendConsumeBuffer::Reset() {
	if (resource_ != nullptr) {
		resource_.Reset();
		counter_.Reset();
		descriptorUAV_.Delete();
	}

	size_ = NULL;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& BaseAppendConsumeBuffer::GetGPUHandleUAV() const {
	StreamLogger::AssertA(resource_ != nullptr, "append consume buffer is not create."); //!< UAVが生成されていない
	return descriptorUAV_.GetGPUHandle();
}

void BaseAppendConsumeBuffer::CreateBuffer(DxObject::Device* device, DxObject::Descriptor&& descriptor, uint32_t size) {

	//!< 引数の保存
	size_          = size;
	descriptorUAV_ = std::move(descriptor);

	//!< resourceの生成[buffer]
	resource_ = DxObject::Resource::CreateBuffer(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		GetByteSize(),
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	//!< resourceの生成[counter]
	counter_ = DxObject::Resource::CreateBuffer(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		sizeof(UINT),
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	{ //!< UAVの作成
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension              = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.NumElements         = GetSize();
		desc.Buffer.StructureByteStride = static_cast<UINT>(GetStride());

		device->GetDevice()->CreateUnorderedAccessView(
			resource_.Get(),
			counter_.Get(),
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
	
}
