#include "Resource.h"
#include "DxResource.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Resource class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Resource::CreateCommitted(
	DxObject::Device* device,
	const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state,
	const std::optional<D3D12_CLEAR_VALUE>& clearValue) {

	//!< resourceの作成
	auto hr = device->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		clearValue.has_value() ? &clearValue.value() : nullptr,
		IID_PPV_ARGS(resource_.GetAddressOf())
	);
	DxObject::Assert(hr, L"resource create failed.");

	//!< stateの保存
	current_ = state;

}

void Resource::Reset() {
	if (resource_ != nullptr) {
		resource_.Reset();
	}
}

void Resource::SetName(const std::wstring& name) const {
	if (resource_ == nullptr) {
		StreamLogger::EngineLog(L"[DXOBJECT Resource] warning | resource is null. cannot set name. name: " + name);
		return; //!< resourceがnullptrの場合は設定できない.
	}

	resource_->SetName(name.c_str());
}

void Resource::Map(void** data, const std::optional<D3D12_RANGE>& range) {
	StreamLogger::AssertA(resource_ != nullptr, "resource is null.");
	resource_->Map(0, range.has_value() ? &range.value() : nullptr, data);
}

void Resource::Unmap(const std::optional<D3D12_RANGE>& range) {
	StreamLogger::AssertA(resource_ != nullptr, "resource is null.");
	resource_->Unmap(0, range.has_value() ? &range.value() : nullptr);
}

std::optional<D3D12_RESOURCE_BARRIER> Resource::GetTransition(D3D12_RESOURCE_STATES state) {
	if (current_ == state) {
		return std::nullopt; //!< stateが同じ場合は遷移しない.
	}

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = current_;
	barrier.Transition.StateAfter  = state;

	current_ = state; //!< stateの更新

	return barrier;
}

void Resource::SetTransition(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state) {
	std::optional<D3D12_RESOURCE_BARRIER> barrier = GetTransition(state);

	if (barrier.has_value()) {
		barriers.push_back(barrier.value());
	}
}

void Resource::Transition(DxObject::CommandContext* context, D3D12_RESOURCE_STATES state) {

	std::optional<D3D12_RESOURCE_BARRIER> barrier = GetTransition(state);

	if (barrier.has_value()) {
		context->GetCommandList()->ResourceBarrier(1, &barrier.value());
	}
}

void Resource::TransitionExplicit(DxObject::CommandContext* context, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter  = after;

	context->GetCommandList()->ResourceBarrier(1, &barrier);

	current_ = after; //!< stateの更新

}

D3D12_GPU_VIRTUAL_ADDRESS Resource::GetGPUVirtualAddress() const {
	StreamLogger::AssertA(resource_ != nullptr, "resource is null.");
	return resource_->GetGPUVirtualAddress();
}

const D3D12_RESOURCE_DESC Resource::GetDesc() const {
	return resource_->GetDesc();
}

Resource Resource::CreateBuffer(
	DxObject::Device* device,
	D3D12_HEAP_TYPE type, size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {

	DxObject::Resource resource = {};

	//!< 制約
	StreamLogger::AssertA(size != 0, "buffer size is zero.");

	//!< propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = type;

	//!< descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = size;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags            = flags;

	//!< resourceの生成
	resource.CreateCommitted(device, prop, desc, state);

	return resource;

}

Resource Resource::CreateTexture(
	DxObject::Device* device,
	D3D12_RESOURCE_DIMENSION dimension, const Vector3ui& size, UINT16 miplevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
	const std::optional<D3D12_CLEAR_VALUE>& clearValue) {

	DxObject::Resource resource = {};

	//!< 制約
	StreamLogger::AssertA(dimension != D3D12_RESOURCE_DIMENSION_BUFFER, "texture dimension is buffer.");

	//!< propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	//!< descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = dimension;
	desc.Width            = size.x;
	desc.Height           = size.y;
	desc.DepthOrArraySize = static_cast<UINT16>(size.z);
	desc.MipLevels        = miplevels;
	desc.Format           = format;
	desc.SampleDesc.Count = 1;
	desc.Flags            = flags;

	//!< resourceの生成
	resource.CreateCommitted(device, prop, desc, state, clearValue);
	
	return resource;
}
