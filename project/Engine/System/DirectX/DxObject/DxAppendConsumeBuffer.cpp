#include "DxAppendConsumeBuffer.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAppendConsumeBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAppendConsumeBuffer::Release() {
	if (resource_ != nullptr) {
		resource_.Reset();
		counter_.Reset();

		counterAddress_ = std::nullopt;
	}

	size_ = NULL;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& BaseAppendConsumeBuffer::GetAppendCousumeGPUHandleUAV() const {
	Exception::Assert(resource_ != nullptr, "append consume buffer is not create."); //!< UAVが生成されていない
	return descriptorUAV_.GetGPUHandle();
}

const D3D12_GPU_VIRTUAL_ADDRESS& BaseAppendConsumeBuffer::GetCounterGPUVirtualAddress() const {
	Exception::Assert(counterAddress_.has_value(), "append consume buffer (counter) is not create."); //!< counterが生成されていない
	return counterAddress_.value();
}

void BaseAppendConsumeBuffer::Create(Device* devices, DescriptorHeaps* descriptorHeaps, uint32_t size, uint32_t counterSize) {

	auto device = devices->GetDevice();

	// 引数の保存
	size_ = size;

	{ //!< resourceの生成

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width            = GetByteSize();
		desc.Height           = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);

	}
	
	{ //!< counterの生成

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width            = sizeof(UINT) * counterSize;
		desc.Height           = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&counter_)
		);

		// counterのアドレスを保存
		counterAddress_ = counter_->GetGPUVirtualAddress();
	}

	{ //!< UAVの生成

		descriptorUAV_ = descriptorHeaps->GetDescriptor(DxObject::kDescriptor_UAV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension              = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.NumElements         = GetSize();
		desc.Buffer.StructureByteStride = static_cast<UINT>(GetStride());

		device->CreateUnorderedAccessView(
			resource_.Get(),
			counter_.Get(),
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
}
