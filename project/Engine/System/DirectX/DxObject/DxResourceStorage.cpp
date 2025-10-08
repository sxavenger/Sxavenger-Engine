#include "DxResourceStorage.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ResourceBuffer::CreateCommittedResource(Device* device, const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state, std::optional<D3D12_CLEAR_VALUE> clearValue) {

	for (size_t i = 0; i < kSwapBufferCount; ++i) {
		auto hr = device->GetDevice()->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			state,
			clearValue.has_value() ? &clearValue.value() : nullptr,
			IID_PPV_ARGS(buffers_[i].GetAddressOf())
		);
		DxObject::Assert(hr, L"resource buffer create committed resource failed.");
	}

}

void ResourceBuffer::SetName(const std::wstring& name) {
	for (size_t i = 0; i < kSwapBufferCount; ++i) {
		buffers_[i]->SetName(std::format(L"{} : [{}]", name, i).c_str());
	}
}

ID3D12Resource* ResourceBuffer::Get(size_t index) const {
	Exception::Assert(index < kSwapBufferCount, "resource buffer index is out of range.");
	return buffers_[index].Get();
}

ID3D12Resource* ResourceBuffer::Get() const {
	return Get(storage_->GetBufferIndex());
}

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ResourceStorage::SwapBuffer() {
	bufferIndex_ = (bufferIndex_ + 1) % ResourceBuffer::kSwapBufferCount;
}
