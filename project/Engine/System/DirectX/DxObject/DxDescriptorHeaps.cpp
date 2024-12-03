#include "DxDescriptorHeaps.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorPool class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorPool::Init(
	Device* devices,
	D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, bool shaderVisible,
	uint32_t descriptorMaxCount) {

	// deviceの取り出し
	ID3D12Device* device = devices->GetDevice();

	// parameterの保存
	descriptorHeapType_ = descriptorHeapType;
	descriptorMaxCount_ = descriptorMaxCount;
	shaderVisible_      = shaderVisible;

	CreateDescriptorHeap(device);

	// handleSizeを取得
	descriptorHandleSize_ = device->GetDescriptorHandleIncrementSize(descriptorHeapType_);
}

void DescriptorPool::Term() {
}

Descriptor DescriptorPool::GetDescriptor() {
	// TODO: multi thread 用にmutexを用意
	Descriptor result = {};

	result.index_ = GetCurrentDescriptorIndex();

	// handleの取得
	result.handles_.first  = GetCPUDescriptorHandle(result.index_);
	result.handles_.second = std::nullopt;

	if (shaderVisible_) {
		result.handles_.second = GetGPUDescriptorHandle(result.index_);
	}

	result.descriptorPool_ = this;

	return result;
}

void DescriptorPool::DeleteDescriptor(Descriptor& descriptor) {
	// TODO: multi thread 用にmutexを用意
	
	//!< 空き配列に挿入
	descriptorDeletedIndices_.emplace(descriptor.index_);
	descriptor.Reset();
}

void DescriptorPool::CreateDescriptorHeap(ID3D12Device* device) {

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type           = descriptorHeapType_;
	desc.NumDescriptors = descriptorMaxCount_;
	desc.Flags          = shaderVisible_
		? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto hr = device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&descriptorHeap_)
	);

	Assert(SUCCEEDED(hr));
}

uint32_t DescriptorPool::GetCurrentDescriptorIndex() {
	uint32_t result = 0;

	if (!descriptorDeletedIndices_.empty()) { //!< 空きindexがある場合
		// 先頭の空きindexの取得
		result = descriptorDeletedIndices_.front(); 
		descriptorDeletedIndices_.pop();

		return result;
	}

	Assert(descriptorIndexCount_ < descriptorMaxCount_, "descriptorHeap max count over.");  //!< 作成した分のDescriptorの要素数を超えている

	// 現在のindexCountを返却
	result = descriptorIndexCount_;

	// 取得するのでインクリメント
	descriptorIndexCount_++;

	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorPool::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE result = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	result.ptr += (descriptorHandleSize_ * index);
	return result;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorPool::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE result = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	result.ptr += (descriptorHandleSize_ * index);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorHeaps class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeaps::Init(Device* device) {

	pools_[DescriptorType::kDescriptor_RTV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_RTV]->Init(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, 24);
	

	pools_[DescriptorType::kDescriptor_DSV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_DSV]->Init(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, 8);

	pools_[DescriptorType::kDescriptor_CBV_SRV_UAV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_CBV_SRV_UAV]->Init(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, 128);

	EngineLog("[_DXOBJECT]::DescriptorHeaps complete init.");
}

void DescriptorHeaps::Term() {
	EngineLog("[_DXOBJECT]::DescriptorHeaps term.");
}

Descriptor DescriptorHeaps::GetDescriptor(DescriptorType type) {
	Assert(type < DescriptorType::kCountOfDescriptorTypeCount, "type is not a valid value.");

	Descriptor result = pools_.at(type)->GetDescriptor();
	result.type_      = type;

	return result;
}

void DescriptorHeaps::DeleteDescriptor(Descriptor& descriptor) {
	if (descriptor.type_.has_value()) {
		pools_.at(descriptor.type_.value())->DeleteDescriptor(descriptor);
	}
}

void DescriptorHeaps::SystemDebugGui() {
	ImGui::SeparatorText("DescriptorPools");
	{ //!< RTV
		auto pool = pools_[kDescriptor_RTV].get();

		float used
			= static_cast<float>(pool->GetUsedDescriptorsCount()) / pool->GetDescriptorMaxCount();

		std::string overlay = std::format(
			"use: {} / max: {}",
			pool->GetUsedDescriptorsCount(),
			pool->GetDescriptorMaxCount()
		);

		ImGui::ProgressBar(used, {}, overlay.c_str());
		ImGui::SameLine();
		ImGui::Text("RTV");
	}

	{ //!< DSV

		auto pool = pools_[kDescriptor_DSV].get();

		float used
			= static_cast<float>(pool->GetUsedDescriptorsCount()) / pool->GetDescriptorMaxCount();

		std::string overlay = std::format(
			"use: {} / max: {}",
			pool->GetUsedDescriptorsCount(),
			pool->GetDescriptorMaxCount()
		);

		ImGui::ProgressBar(used, {}, overlay.c_str());
		ImGui::SameLine();
		ImGui::Text("DSV");
	}

	{ //!< SRV_CBV_UAV

		auto pool = pools_[kDescriptor_CBV_SRV_UAV].get();

		float used
			= static_cast<float>(pool->GetUsedDescriptorsCount()) / pool->GetDescriptorMaxCount();

		std::string overlay = std::format(
			"use: {} / max: {}",
			pool->GetUsedDescriptorsCount(),
			pool->GetDescriptorMaxCount()
		);

		ImGui::ProgressBar(used, {}, overlay.c_str());
		ImGui::SameLine();
		ImGui::Text("CBV_SRV_UAV");
	}

}
