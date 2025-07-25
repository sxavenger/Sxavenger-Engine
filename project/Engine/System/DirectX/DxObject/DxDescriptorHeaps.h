#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptor.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

// c++
#include <queue>
#include <memory>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorPool class
////////////////////////////////////////////////////////////////////////////////////////////
class DescriptorPool {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DescriptorPool()  = default;
	~DescriptorPool() { Term(); }

	void Init(
		Device* device,
		D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, bool shaderVisible,
		uint32_t descriptorMaxCount
	);

	void Term();

	//* descriptor option *//

	_NODISCARD Descriptor GetDescriptor();

	void DeleteDescriptor(Descriptor& descriptor);

	//* getter *//

	ID3D12DescriptorHeap* const GetDescriptorHeap() const {
		return descriptorHeap_.Get();
	}

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
	
	std::queue<uint32_t> descriptorDeletedIndices_; //!< 動的に消されたDescriptorのIndexの格納先

	//* config *//

	bool shaderVisible_ = false;

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
class DescriptorHeaps
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DescriptorHeaps() = default;
	~DescriptorHeaps() { Term(); }

	void Init(Device* device);

	void Term();

	//* descriptor option *//

	_NODISCARD Descriptor GetDescriptor(DescriptorType type);

	void DeleteDescriptor(Descriptor& descriptor);

	//* getter *//

	const uint32_t GetDescriptorMaxCount(DescriptorType type) const {
		return pools_.at(type)->GetDescriptorMaxCount();
	}

	ID3D12DescriptorHeap* const GetDescriptorHeap(DescriptorType type) const {
		return pools_.at(type)->GetDescriptorHeap();
	}

	DescriptorPool* const GetDescriptorPool(DescriptorType type) const {
		return pools_.at(type).get();
	}

	//* imgui option *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const std::array<UINT, DescriptorType::kCountOfDescriptorTypeCount> kDescriptorCount_ = {
		24, //!< RTV
		8,  //!< DSV
		512 //!< CBV_SRV_UAV
	};

	std::array<std::unique_ptr<DescriptorPool>, DescriptorType::kCountOfDescriptorTypeCount> pools_;

};

_DXOBJECT_NAMESPACE_END
