#include "DxResourceStorage.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Buffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ResourceBuffer::Buffer::AssignAddress() {
	address = (resource != nullptr) ? std::make_optional(resource->GetGPUVirtualAddress()) : std::nullopt;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ResourceBuffer::SetName(const std::wstring& name) {
	for (size_t i = 0; i < kSwapBufferCount; ++i) {
		buffers_[i].resource->SetName(std::format(L"{} : [{}]", name, i).c_str());
	}
}

ID3D12Resource* ResourceBuffer::Get(size_t index) const {
	Exception::Assert(index < kSwapBufferCount, "resource buffer index is out of range.");
	return buffers_[index].resource.Get();
}

ID3D12Resource* ResourceBuffer::Get() const {
	return Get(storage_->GetBufferIndex());
}

const D3D12_GPU_VIRTUAL_ADDRESS& ResourceBuffer::GetGPUVirtualAddress(size_t index) const {
	Exception::Assert(index < kSwapBufferCount, "resource buffer index is out of range.");
	Exception::Assert(buffers_[index].address.has_value(), "resource buffer address is not available.");
	return buffers_[index].address.value();
}

const D3D12_GPU_VIRTUAL_ADDRESS& ResourceBuffer::GetGPUVirtualAddress() const {
	return GetGPUVirtualAddress(storage_->GetBufferIndex());
}

void ResourceBuffer::CreateCommittedResource(
	Device* device,
	const D3D12_HEAP_PROPERTIES& prop,
	const D3D12_RESOURCE_DESC& desc,
	D3D12_RESOURCE_STATES state,
	D3D12_HEAP_FLAGS flags,
	const std::optional<D3D12_CLEAR_VALUE>& clearValue) {

	for (size_t i = 0; i < kSwapBufferCount; ++i) {
		auto hr = device->GetDevice()->CreateCommittedResource(
			&prop,
			flags,
			&desc,
			state,
			clearValue.has_value() ? &clearValue.value() : nullptr,
			IID_PPV_ARGS(&buffers_[i].resource)
		);
		DxObject::Assert(hr, L"create committed resource failed.");

		buffers_[i].AssignAddress();
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ResourceBuffer> ResourceStorage::CreateBuffer() {
	 container_.emplace_back(std::make_shared<ResourceBuffer>(this, container_.end()));
	 return container_.back();
}

void ResourceStorage::Delete(const ResourceBuffer::Iterator& iterator) {
	destroyQueue_.emplace(iterator);
}

void ResourceStorage::Destroy() {
	while (!destroyQueue_.empty()) {
		container_.erase(destroyQueue_.front());
		destroyQueue_.pop();
	}
}

void ResourceStorage::SwapBuffer() {
	bufferIndex_ = (bufferIndex_ + 1) % ResourceBuffer::kSwapBufferCount;
}
