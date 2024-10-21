#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

// c++
#include <deque>
#include <memory>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Devices;

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorPool class
////////////////////////////////////////////////////////////////////////////////////////////
class DescriptorPool {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DescriptorPool() = default;

	DescriptorPool(
		Devices* devices,
		D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, uint32_t descriptorMaxCount
	);

	~DescriptorPool() { Term(); }

	void Init(
		Devices* devices,
		D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, uint32_t descriptorMaxCount
	);

	void Term();

	//* descriptor option *//

	Descriptor GetDescriptor();

	void DeleteDescriptor(Descriptor& descriptor);

	//* getter *//

	ID3D12DescriptorHeap* const GetDescriptorHeap() const { return descriptorHeap_.Get(); }

	const uint32_t GetDescriptorMaxCount() const {
		return descriptorMaxCount_;
	}

	const uint32_t GetUsedDescriptorsCount() const {
		return static_cast<uint32_t>(descriptorIndexCount_ - descriptorDeletedIndices_.size());
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptorHeap *//

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	D3D12_DESCRIPTOR_HEAP_TYPE   descriptorHeapType_;
	UINT                         descriptorHandleSize_;

	//* descriptorPool *//

	uint32_t descriptorMaxCount_;   //!< descriptorの最大数
	uint32_t descriptorIndexCount_ = 0;
	
	std::deque<uint32_t> descriptorDeletedIndices_; //!< 動的に消されたDescriptorのIndexの格納先

	//* config *//

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDescriptorHeap(ID3D12Device* device);

	uint32_t GetCurrentDescriptorIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorHeaps class
////////////////////////////////////////////////////////////////////////////////////////////
class DescriptorHeaps {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DescriptorHeaps(Devices* device) { Init(device); }

	~DescriptorHeaps() { Term(); }

	void Init(Devices* device);

	void Term();

	//* descriptor option *//

	Descriptor GetDescriptor(DescriptorType type);

	void DeleteDescriptor(Descriptor& descriptor);

	//* getter *//

	const uint32_t GetDescriptorMaxCount(DescriptorType type) const;

	ID3D12DescriptorHeap* const GetDescriptorHeap(DescriptorType type) const;

	DescriptorPool* const GetDescriptorPool(DescriptorType type) const { return pools_.at(type).get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::unique_ptr<DescriptorPool>, DescriptorType::kDescriptorHeapCount> pools_;



};

_DXOBJECT_NAMESPACE_END