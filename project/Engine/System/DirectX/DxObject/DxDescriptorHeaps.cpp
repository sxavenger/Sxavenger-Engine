#include "DxDescriptorHeaps.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/Utility/StreamLogger.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

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

	StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool] descriptor heap type: {}, visibility: {}, count: {}", magic_enum::enum_name(descriptorHeapType), shaderVisible, descriptorMaxCount));
}

void DescriptorPool::Term() {
	if (GetUsedDescriptorsCount() != 0) {
		StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool {}] warning: descriptor leak detected. used count: {}.", magic_enum::enum_name(descriptorHeapType_), GetUsedDescriptorsCount()));
	}
}

Descriptor DescriptorPool::GetDescriptor() {
	std::unique_lock<std::mutex> lock(mutex_);
	
	Descriptor result = {};

	result.index_ = GetCurrentDescriptorIndex();

	// handleの取得
	result.handles_.first  = GetCPUDescriptorHandle(result.index_);
	result.handles_.second = std::nullopt;

	if (shaderVisible_) {
		result.handles_.second = GetGPUDescriptorHandle(result.index_);
	}

	result.descriptorPool_ = this;

	StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool {}] create descriptor index: {}.", magic_enum::enum_name(descriptorHeapType_), result.index_));
	return result;
}

void DescriptorPool::DeleteDescriptor(Descriptor& descriptor) {
	std::unique_lock<std::mutex> lock(mutex_);
	
	//!< 空き配列に挿入
	descriptorFreeIndices_.emplace(descriptor.index_);
	StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool {}] delete descriptor index: {}.", magic_enum::enum_name(descriptorHeapType_), descriptor.index_));

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
	DxObject::Assert(hr, L"descriptor heap create failed.");
}

uint32_t DescriptorPool::GetCurrentDescriptorIndex() {
	uint32_t result = 0;

	if (!descriptorFreeIndices_.empty()) { //!< 空きindexがある場合
		// 先頭の空きindexの取得
		result = descriptorFreeIndices_.front();
		descriptorFreeIndices_.pop();

		return result;
	}

	StreamLogger::AssertA(descriptorIndexCount_ < descriptorMaxCount_, std::format("descriptor heap max count over. type: {}", magic_enum::enum_name(descriptorHeapType_).data()));  //!< 作成した分のDescriptorの要素数を超えている

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
	pools_[DescriptorType::kDescriptor_RTV]->Init(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, Configuration::GetConfig().descriptorCount_RTV);

	pools_[DescriptorType::kDescriptor_DSV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_DSV]->Init(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, Configuration::GetConfig().descriptorCount_DSV);

	pools_[DescriptorType::kDescriptor_CBV_SRV_UAV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_CBV_SRV_UAV]->Init(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, Configuration::GetConfig().descriptorCount_SRV_CBV_UAV);

	StreamLogger::EngineLog("[DXOBJECT DescriptorHeaps] complete initialize.");
}

void DescriptorHeaps::Term() {
	StreamLogger::EngineLog("[DXOBJECT DescriptorHeaps] term.");
}

Descriptor DescriptorHeaps::GetDescriptor(DescriptorType type) {
	StreamLogger::AssertA(type < DescriptorType::kCountOfDescriptorTypeCount, "type is not a valid value.");

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
