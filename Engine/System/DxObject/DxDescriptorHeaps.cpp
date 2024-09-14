#include "DxDescriptorHeaps.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include "DxDevices.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorPool class methods
////////////////////////////////////////////////////////////////////////////////////////////

DescriptorPool::DescriptorPool(Devices* devices, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, uint32_t descriptorMaxCount) {
	Init(devices, descriptorHeapType, descriptorMaxCount);
}

void DescriptorPool::Init(
	Devices* devices,
	D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, uint32_t descriptorMaxCount) {

	// deviceの取り出し
	ID3D12Device* device = devices->GetDevice();

	// parameterの保存
	descriptorHeapType_ = descriptorHeapType;
	descriptorMaxCount_ = descriptorMaxCount;

	CreateDescriptorHeap(device);

	// handleSizeを取得
	descriptorHandleSize_ = device->GetDescriptorHandleIncrementSize(descriptorHeapType_);
}

void DescriptorPool::Term() {
}

Descriptor DescriptorPool::GetDescriptor() {

	Descriptor result;

	// indexの取得
	result.index = GetCurrentDescriptorIndex();

	// handleの取得
	result.handles.first  = GetCPUDescriptorHandle(result.index);
	result.handles.second = std::nullopt;

	if (descriptorHeapType_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
		result.handles.second = GetGPUDescriptorHandle(result.index);
	}

	return result;
}

void DescriptorPool::DeleteDescriptor(Descriptor& descriptor) {

	//!< 空き配列に挿入
	descriptorDeletedIndices_.push_back(descriptor.index);

	// 消したのでdescriptorの要素削除
	descriptor.Term();
}

void DescriptorPool::CreateDescriptorHeap(ID3D12Device* device) {

	bool shaderVisible = (descriptorHeapType_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); //!< 仮でCBV_SRV_UAVの場合はshaderで使うとする

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type           = descriptorHeapType_;
	desc.NumDescriptors = descriptorMaxCount_;
	desc.Flags          = shaderVisible
		? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto hr = device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(&descriptorHeap_)
	);

	Assert(SUCCEEDED(hr));

}

uint32_t DescriptorPool::GetCurrentDescriptorIndex() {
	
	uint32_t result = 0;

	if (!descriptorDeletedIndices_.empty()) {  //!< 空きindexがある場合

		// 先頭の空きindexの取得
		result = descriptorDeletedIndices_.front(); 

		// 先頭の空きindexを渡すので先頭をpop
		descriptorDeletedIndices_.pop_front();

		return result;
	}

	Assert(descriptorIndexCount_ < descriptorMaxCount_);  //!< 作成した分のDescriptorの要素数を超えている

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

void DescriptorHeaps::Init(Devices* devices) {

	pools_[DescriptorType::RTV]
		= std::make_unique<DescriptorPool>(devices, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 12);

	pools_[DescriptorType::DSV]
		= std::make_unique<DescriptorPool>(devices, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

	pools_[DescriptorType::CBV_SRV_UAV]
		= std::make_unique<DescriptorPool>(devices, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128);
	
	Log("[_DXOBJECT]::DescriptorHeaps complete init.");
}

void DescriptorHeaps::Term() {
}

Descriptor DescriptorHeaps::GetDescriptor(DescriptorType type) {
	Assert(type < DescriptorType::kDescriptorHeapCount); //!< 範囲外参照

	Descriptor result = pools_.at(type)->GetDescriptor();
	result.type = type;

	return result;
}

void DescriptorHeaps::DeleteDescriptor(Descriptor& descriptor) {
	if (descriptor.type == DescriptorType::kDescriptorHeapCount) { //!< 無効値
		return;
	}

	pools_.at(descriptor.type)->DeleteDescriptor(descriptor);
}

const uint32_t DescriptorHeaps::GetDescriptorMaxCount(DescriptorType type) const {
	return pools_.at(type)->GetDescriptorMaxCount();
}

ID3D12DescriptorHeap* const DescriptorHeaps::GetDescriptorHeap(DescriptorType type) const {
	return pools_.at(type)->GetDescriptorHeap();
}
