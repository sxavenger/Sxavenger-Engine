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
	DescriptorType type, bool shaderVisible,
	uint32_t capacity) {

	// deviceの取り出し
	ID3D12Device* device = devices->GetDevice();

	// parameterの保存
	type_               = type;
	shaderVisible_      = shaderVisible;

	//!< allocatorの初期化
	allocator_.Capacity(capacity);

	CreateDescriptorHeap(device, DescriptorPool::GetDescriptorHeapType(type));
	StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool] descriptor heap type: {}, visibility: {}, capacity: {}", magic_enum::enum_name(type), shaderVisible, capacity));

}

void DescriptorPool::Term() {
	if (GetUsedDescriptorsCount() != 0) {
		StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool {}] warning: descriptor leak detected. used count: {}.", magic_enum::enum_name(type_), GetUsedDescriptorsCount()));
	}
}

Descriptor DescriptorPool::GetDescriptor() {
	std::unique_lock<std::mutex> lock(mutex_);
	
	Descriptor::Handle handle(type_);

	//!< indexの取得
	handle.index = GetCurrentDescriptorIndex();

	//!< CPU/GPUハンドルの取得
	handle.cpu = GetCPUDescriptorHandle(handle.index);

	if (shaderVisible_) {
		handle.gpu = GetGPUDescriptorHandle(handle.index);
	}

	//!< descriptorの作成
	Descriptor descriptor = {};
	descriptor.pool_ = this; //!< HACK: 生ptrの保存
	descriptor.handle_.emplace(std::move(handle));

	StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool {}] create descriptor index: {}.", magic_enum::enum_name(handle.type), handle.index));
	return descriptor;
}

void DescriptorPool::DeleteDescriptor(Descriptor& descriptor) {
	std::unique_lock<std::mutex> lock(mutex_);

	{
		const Descriptor::Handle& handle = descriptor.handle_.value();

		//!< indexの解放
		allocator_.Free(handle.index);
		StreamLogger::EngineLog(std::format("[DXOBJECT DescriptorPool {}] delete descriptor index: {}.", magic_enum::enum_name(handle.type), handle.index));
	}
	
	//!< descriptorのリセット
	descriptor.handle_ = std::nullopt;
	descriptor.pool_   = nullptr;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorPool::GetDescriptorHeapType(DescriptorType type) {
	switch (type) {
		case kDescriptor_RTV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		case kDescriptor_DSV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		case kDescriptor_CBV_SRV_UAV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}

	StreamLogger::Exception("type is not a valid value.");
}

void DescriptorPool::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type) {

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type           = type;
	desc.NumDescriptors = allocator_.GetCapacity();
	desc.Flags          = shaderVisible_
		? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto hr = device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&descriptorHeap_)
	);
	DxObject::Assert(hr, L"descriptor heap create failed.");

	//!< sizeの保存
	descriptorHandleSize_ = device->GetDescriptorHandleIncrementSize(type);
}

uint32_t DescriptorPool::GetCurrentDescriptorIndex() {
	StreamLogger::AssertA(allocator_.CheckAvailable(), std::format("descriptor heap max count over. type: {}", magic_enum::enum_name(type_).data()));  //!< 作成した分のDescriptorの要素数を超えている
	return allocator_.Get();
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
	pools_[DescriptorType::kDescriptor_RTV]->Init(device, DescriptorType::kDescriptor_RTV, false, Configuration::GetConfig().descriptorCount_RTV);

	pools_[DescriptorType::kDescriptor_DSV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_DSV]->Init(device, DescriptorType::kDescriptor_DSV, false, Configuration::GetConfig().descriptorCount_DSV);

	pools_[DescriptorType::kDescriptor_CBV_SRV_UAV] = std::make_unique<DescriptorPool>();
	pools_[DescriptorType::kDescriptor_CBV_SRV_UAV]->Init(device, DescriptorType::kDescriptor_CBV_SRV_UAV, true, Configuration::GetConfig().descriptorCount_SRV_CBV_UAV);

	StreamLogger::EngineLog("[DXOBJECT DescriptorHeaps] complete initialize.");
}

void DescriptorHeaps::Term() {
	StreamLogger::EngineLog("[DXOBJECT DescriptorHeaps] term.");
}

Descriptor DescriptorHeaps::GetDescriptor(DescriptorType type) {
	StreamLogger::AssertA(type < DescriptorType::kCountOfDescriptorTypeCount, "type is not a valid value.");
	return pools_[type]->GetDescriptor();
}

void DescriptorHeaps::DeleteDescriptor(Descriptor& descriptor) {
	if (descriptor.handle_.has_value()) {
		pools_[descriptor.handle_->type]->DeleteDescriptor(descriptor);
	}
}

void DescriptorHeaps::SystemDebugGui() {
	ImGui::SeparatorText("DescriptorPools");
	{ //!< RTV
		auto pool = pools_[kDescriptor_RTV].get();

		if (pool->GetDescriptorCapacity() != 0) {
			std::string overlay = std::format(
				"use: {} / max: {}",
				pool->GetUsedDescriptorsCount(),
				pool->GetDescriptorCapacity()
			);

			float used = static_cast<float>(pool->GetUsedDescriptorsCount()) / pool->GetDescriptorCapacity();
			ImGui::ProgressBar(used, {}, overlay.c_str());

		} else {
			ImGui::ProgressBar(0.0f, {}, "capacity is 0");
		}

		ImGui::SameLine();
		ImGui::Text("RTV");
	}

	{ //!< DSV

		auto pool = pools_[kDescriptor_DSV].get();

		if (pool->GetDescriptorCapacity() != 0) {
			std::string overlay = std::format(
				"use: {} / max: {}",
				pool->GetUsedDescriptorsCount(),
				pool->GetDescriptorCapacity()
			);

			float used = static_cast<float>(pool->GetUsedDescriptorsCount()) / pool->GetDescriptorCapacity();
			ImGui::ProgressBar(used, {}, overlay.c_str());

		} else {
			ImGui::ProgressBar(0.0f, {}, "capacity is 0");
		}

		ImGui::SameLine();
		ImGui::Text("DSV");
	}

	{ //!< SRV_CBV_UAV

		auto pool = pools_[kDescriptor_CBV_SRV_UAV].get();

		if (pool->GetDescriptorCapacity() != 0) {

			std::string overlay = std::format(
				"use: {} / max: {}",
				pool->GetUsedDescriptorsCount(),
				pool->GetDescriptorCapacity()
			);

			float used = static_cast<float>(pool->GetUsedDescriptorsCount()) / pool->GetDescriptorCapacity();
			ImGui::ProgressBar(used, {}, overlay.c_str());

		} else {
			ImGui::ProgressBar(0.0f, {}, "capacity is 0");
		}

		ImGui::SameLine();
		ImGui::Text("CBV_SRV_UAV");
	}

}
